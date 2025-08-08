/*
** client.cpp -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <vector>
#include <tuple>
#include "Graph.hpp"
#include <string>
#include <iostream>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 1000 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	const char* hostname = "127.0.0.1";
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(hostname, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}
		inet_ntop(p->ai_family,
			get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
		printf("client: attempting connection to %s\n", s);
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}
	inet_ntop(p->ai_family,
			get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connected to %s\n", s);
	freeaddrinfo(servinfo);

	// קלט האם הגרף מכוון
	int isDirected;
	printf("Is the graph directed? (0=No, 1=Yes): ");
	if (scanf("%d", &isDirected) != 1 || (isDirected != 0 && isDirected != 1)) {
		printf("Invalid input\n");
		return 1;
	}

	int vertices, edges;
	printf("Enter number of vertices: ");
	if (scanf("%d", &vertices) != 1 || vertices <= 0) {
		fprintf(stderr, "Error: Number of vertices must be positive integer.\n");
		return 1;
	}
	printf("Enter number of edges: ");
	if (scanf("%d", &edges) != 1 || edges < 0) {
		fprintf(stderr, "Error: Number of edges must be non-negative integer.\n");
		return 1;
	}

	std::vector<std::tuple<int,int,int>> edgeList;
	for (int i = 0; i < edges; ++i) {
		int u, v, w;
		printf("Enter edge %d (u v w): ", i+1);
		if (scanf("%d %d %d", &u, &v, &w) != 3 || u < 0 || u >= vertices || v < 0 || v >= vertices || w <= 0) {
			fprintf(stderr, "Error: Invalid edge or weight.\n");
			return 1;
		}
		if (u == v) {
			fprintf(stderr, "Warning: Self-loops not allowed.\n");
			return 1;
		}
		bool duplicate = false;
		for (auto& e : edgeList) {
			if ((std::get<0>(e) == u && std::get<1>(e) == v) || (!isDirected && std::get<0>(e) == v && std::get<1>(e) == u)) duplicate = true;
		}
		if (duplicate) {
			fprintf(stderr, "Warning: Duplicate edge (%d,%d) not allowed.\n", u, v);
			return 1;
		}
		edgeList.push_back(std::make_tuple(u, v, w));
	}

	// Build adjacency matrix text
	std::vector<std::vector<int>> adj(vertices, std::vector<int>(vertices, 0));
	for (auto& e : edgeList) {
		int u = std::get<0>(e), v = std::get<1>(e), w = std::get<2>(e);
		adj[u][v] = w;
		if (!isDirected) adj[v][u] = w;
	}

	char graphbuf[4096] = {0};
	sprintf(graphbuf, "%d\n", isDirected); // Send directed flag
	for (int i = 0; i < vertices; ++i) {
		for (int j = 0; j < vertices; ++j) {
			char num[8];
			sprintf(num, "%d", adj[i][j]);
			strcat(graphbuf, num);
			if (j+1 < vertices) strcat(graphbuf, " ");
		}
		strcat(graphbuf, "\n");
	}

	// Send graph to server
	if (send(sockfd, graphbuf, strlen(graphbuf), 0) == -1) {
		perror("send");
		return 1;
	}

	// Get algorithm choice from user
	std::string algorithm;
	std::cout << "Choose algorithm (mst, maxflow, scc, clique): ";
	std::cin >> algorithm;
	// Send algorithm name to server
	send(sockfd, algorithm.c_str(), algorithm.size(), 0);
	// Receive result from server
	char result[1024] = {0};
	recv(sockfd, result, sizeof(result), 0);
	std::cout << "Result from server: " << result << std::endl;
	// Finish
	close(sockfd);
	return 0;
}
