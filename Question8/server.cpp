/*
** server.cpp -- a stream socket server demo
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
#include "Graph.hpp"
#include "GraphAlgorithm.hpp"
#include "GraphAlgorithmFactory.hpp"

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	// listen on sock_fd, new connection on new_fd
	int sockfd, new_fd;
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address info
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
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

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr,
				&sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

		// get data from the client
		std::string full_data;
		char buf[4096];
		int total_bytes = 0;
		while (true) {
			int numbytes = recv(new_fd, buf, sizeof(buf)-1, 0);
			if (numbytes <= 0) break;
			buf[numbytes] = '\0';
			full_data += buf;
			total_bytes += numbytes;
			// Stop if we have enough lines (seed + 4 params + num_edges)
			int line_count = 0;
			for (char c : full_data) if (c == '\n') ++line_count;
			// We need at least 5 + num_edges lines
			if (line_count >= 5) {
				std::istringstream iss(full_data);
				std::string tmp;
				std::getline(iss, tmp); // seed
				std::getline(iss, tmp); // directed
				std::getline(iss, tmp); // vertices
				int num_vertices = atoi(tmp.c_str());
				std::getline(iss, tmp); // edges
				int num_edges = atoi(tmp.c_str());
				// If we have enough lines for all edges, break
				if (line_count >= 5 + num_edges) break;
			}
		}
		std::istringstream iss(full_data);
		std::string line;
		std::getline(iss, line); // seed
		int seed = atoi(line.c_str());
		std::getline(iss, line); // directed flag
		int directed = atoi(line.c_str());
		std::getline(iss, line); // vertices
		int num_vertices = atoi(line.c_str());
		std::getline(iss, line); // edges
		int num_edges = atoi(line.c_str());
		std::getline(iss, line); // max_weight
		int max_weight = atoi(line.c_str());

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
		// Print the graph that was built
		if (seed != -1)
			printf("[Server] Built graph with seed %d:\n", seed);
		else
			printf("[Server] Built graph (manual mode):\n");
		graph.printGraph();

		// Receive algorithm name
		char algoName[64] = {0};
		recv(new_fd, algoName, sizeof(algoName), 0);
		std::string algorithm(algoName);
		// No need to check for weighted/directed graph, all graphs are directed and weighted by default
		GraphAlgorithm* algo = GraphAlgorithmFactory::create(algorithm);
		if (!algo) {
			std::string err = "Error: Unsupported algorithm.";
			send(new_fd, err.c_str(), err.size(), 0);
		} else {
			std::string result = algo->run(graph);
			send(new_fd, result.c_str(), result.size(), 0);
			delete algo;
		}
		// Finish
		close(new_fd);
		printf("server: client disconnected\n");
	}

	return 0;
}
