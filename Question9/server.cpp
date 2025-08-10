/*
** server.cpp -- a stream socket server demo with Leader-Follower
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>
#include <sstream>
#include <set>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Graph.hpp"
#include "GraphAlgorithm.hpp"
#include "GraphAlgorithmFactory.hpp"
#include <memory>
#include "MSTAlgorithm.hpp"
#include "MaxFlowAlgorithm.hpp"
#include "SCCAlgorithm.hpp"
#include "CliqueCountAlgorithm.hpp"

#define PORT "3490"
#define BACKLOG 10
#define NUM_THREADS 4

// Leader-Follower synchronization
std::mutex lf_mtx;
std::condition_variable lf_cv;
bool leader_token = true;

// Helper function: run all algorithms and build a single response
static std::string run_all_algorithms(const Graph::Graph& g) {
    std::ostringstream out;
    std::vector<std::string> names = {"mst","maxflow","scc","clique"};
    for (const auto& name : names) {
        std::unique_ptr<GraphAlgorithm> algo(GraphAlgorithmFactory::create(name));
        std::string res = algo->run(g);
        out << name << ": " << res << "\n";
    }
    return out.str();
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Worker thread function - this is where the Leader-Follower magic happens
void worker_thread(int sockfd) {
    while(1) {
        struct sockaddr_storage their_addr;
        socklen_t sin_size = sizeof their_addr;
        char s[INET6_ADDRSTRLEN];
        int new_fd;

        // STEP 1: Acquire leader role
		std::unique_lock<std::mutex> lock(lf_mtx);
		while (!leader_token) {
			lf_cv.wait(lock);  // Wait as follower
		}
		leader_token = false;    // Become leader
		lock.unlock();

        // STEP 2: Accept connection (only leader does this)
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            // Restore leader token on error
            lf_mtx.lock();
            leader_token = true;
            lf_cv.notify_one();
            lf_mtx.unlock();
            continue;
        }

        // STEP 3: Immediately promote next leader
        lf_mtx.lock();
        leader_token = true;
        lf_cv.notify_one();      // Wake up one follower to become new leader
        lf_mtx.unlock();

        // STEP 4: Handle this client (current thread becomes worker)
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);

        // Get data from client
        std::string full_data;
        char buf[4096];
        while (true) {
            int numbytes = recv(new_fd, buf, sizeof(buf)-1, 0);
            if (numbytes <= 0) break;
            buf[numbytes] = '\0';
            full_data += buf;
            
            // Check if we have complete message
            int line_count = 0;
            for (char c : full_data) if (c == '\n') ++line_count;
            if (line_count >= 5) {
                std::istringstream iss(full_data);
                std::string tmp;
                std::getline(iss, tmp); // seed
                std::getline(iss, tmp); // directed
                std::getline(iss, tmp); // vertices
                std::getline(iss, tmp); // edges
                int num_edges = atoi(tmp.c_str());
                if (line_count >= 5 + num_edges) break;
            }
        

			// Parse input
			std::istringstream iss(full_data);
			std::string line;
			std::getline(iss, line); int seed = atoi(line.c_str());
			std::getline(iss, line); int directed = atoi(line.c_str());
			std::getline(iss, line); int num_vertices = atoi(line.c_str());
			std::getline(iss, line); int num_edges = atoi(line.c_str());

			// Build graph
			Graph::Graph graph(num_vertices, directed);
			std::set<std::pair<int,int>> used_edges;
			for (int i = 0; i < num_edges; ++i) {
				std::getline(iss, line);
				std::istringstream lss(line);
				int u, v, w;
				lss >> u >> v >> w;
				if (u < 0 || u >= num_vertices || v < 0 || v >= num_vertices || w <= 0) {
					printf("[Server] Error: Invalid edge (%d,%d,%d)\n", u, v, w);
					continue;
				}
				if (u == v) {
					printf("[Server] Warning: Self-loop (%d,%d) ignored.\n", u, v);
					continue;
				}
				if (used_edges.count({u, v})) {
					printf("[Server] Warning: Duplicate edge (%d,%d) ignored.\n", u, v);
					continue;
				}
				graph.addEdge(u, v, w);
				used_edges.insert({u, v});
			}

			// Print graph
			if (seed != -1)
				printf("[Server] Built graph with seed %d:\n", seed);
			else
				printf("[Server] Built graph (manual mode):\n");
			graph.printGraph();

			// Run algorithms and send result
			std::string result = run_all_algorithms(graph);
			send(new_fd, result.c_str(), result.size(), 0);
			
			// Close connection
			close(new_fd);
			printf("server: client disconnected\n");
			
			// Thread returns to follower pool and may become leader again
		}
	}
}

int main(void) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int yes = 1;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // Create and bind socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }

    freeaddrinfo(servinfo);
    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    // Create thread pool for Leader-Follower
    std::vector<std::thread> workers;
    for (int i = 0; i < NUM_THREADS; ++i) {
        workers.emplace_back(worker_thread, sockfd);
    }

    // Wait for all threads to finish (they never will in this case)
    for (auto& t : workers) {
        t.join();
    }

    return 0;
}