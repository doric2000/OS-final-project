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
		char buf[4096];
		int numbytes = recv(new_fd, buf, sizeof(buf)-1, 0);
		if (numbytes <= 0) {
			perror("recv");
			close(new_fd);
			continue;
		}
		buf[numbytes] = '\0';

		// Receive directed flag and adjacency matrix rows
		std::istringstream iss(buf);
		std::string line;
		int isDirected = 0;
		std::getline(iss, line);
		isDirected = std::stoi(line);
		std::vector<std::vector<int>> adj;
		while (std::getline(iss, line)) {
			std::istringstream lss(line);
			std::vector<int> row;
			int val;
			while (lss >> val) row.push_back(val);
			if (!row.empty()) adj.push_back(row);
		}
		int num_vertices = adj.size();
		Graph::Graph graph(num_vertices, isDirected);
		for (int i = 0; i < num_vertices; ++i) {
			for (int j = 0; j < adj[i].size(); ++j) {
				if (adj[i][j] > 0) {
					graph.addEdge(i, j, adj[i][j]);
				}
			}
		}

		// Receive algorithm name
		char algoName[64] = {0};
		recv(new_fd, algoName, sizeof(algoName), 0);
		std::string algorithm(algoName);
		// Block unsupported algorithms for undirected/unweighted graphs
		bool isWeightedDirectedRequired = (algorithm == "maxflow" || algorithm == "scc");
		if (isWeightedDirectedRequired && !graph.isDirected()) {
			std::string err = "Error: This algorithm requires a directed graph.";
			send(new_fd, err.c_str(), err.size(), 0);
		} else {
			GraphAlgorithm* algo = GraphAlgorithmFactory::create(algorithm);
			if (!algo) {
				std::string err = "Error: Unsupported algorithm.";
				send(new_fd, err.c_str(), err.size(), 0);
			} else {
				std::string result = algo->run(graph);
				send(new_fd, result.c_str(), result.size(), 0);
				delete algo;
			}
		}
		// Finish
		close(new_fd);
		printf("server: client disconnected\n");
	}

	return 0;
}
