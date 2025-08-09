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
#include <getopt.h>
#include <random>
#include <set>
#include <sstream>

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

	int seed = time(NULL);
	// getopt setup
	int opt;
	int mode = -1; // -1=unset, 0=manual, 1=random
	int vertices = 0, edges = 0, max_weight = 10;
	bool error = false;
	while ((opt = getopt(argc, argv, "rmn:e:w:s:")) != -1) {
		switch (opt) {
			case 'r': mode = 1; break;
			case 'm': mode = 0; break;
			case 'n': vertices = atoi(optarg); break;
			case 'e': edges = atoi(optarg); break;
			case 'w': max_weight = atoi(optarg); break;
			case 's': seed = atoi(optarg); break;
			default: error = true; break;
		}
	}
	if (mode == -1 || vertices <= 0 || error || (mode == 1 && edges <= 0)) {
		fprintf(stderr,
			"Usage: %s [-r|-m] -n <vertices> -e <edges> [-w <max_weight>] [-s <seed>]\n"
			"  -r : random graph mode (requires -n, -e, -w) [-s <seed>]\n"
			"  -m : manual graph mode (requires -n, -e)\n"
			"  -n : number of vertices (>0)\n"
			"  -e : number of edges (>0)\n"
			"  -w : max edge weight (random mode, default 10)\n"
			"  -s : random seed (optional, random mode only, default is current time)\n",
			argv[0]);
		return 1;
	}

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

	// Build graph (always directed & weighted)
	std::vector<std::tuple<int,int,int>> edgeList;
	if (mode == 1) {
		// Random graph: generate 'edges' random directed edges with random weights
		if (edges <= 0 || edges > vertices * (vertices - 1)) {
			fprintf(stderr, "Error: Number of edges must be in [1, V*(V-1)] for directed graph without self-loops.\n");
			return 1;
		}
		std::mt19937 gen(seed);
		std::uniform_int_distribution<> weight_dist(1, max_weight);
		std::set<std::pair<int,int>> used_edges;
		int generated = 0;
		while (generated < edges) {
			int u = gen() % vertices;
			int v = gen() % vertices;
			if (u == v) continue; // No self-loops
			if (used_edges.count({u, v})) continue; // No duplicate edges
			int w = weight_dist(gen);
			edgeList.push_back(std::make_tuple(u, v, w));
			used_edges.insert({u, v});
			++generated;
		}
	} else {
		// Manual graph
		if (vertices <= 0) {
			fprintf(stderr, "Error: Number of vertices must be positive in manual mode.\n");
			return 1;
		}
		if (edges <= 0) {
			fprintf(stderr, "Error: Number of edges must be provided as a command-line argument (-e <edges>) in manual mode.\n");
			return 1;
		}
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
				if (std::get<0>(e) == u && std::get<1>(e) == v) duplicate = true;
			}
			if (duplicate) {
				fprintf(stderr, "Warning: Duplicate edge (%d,%d) not allowed.\n", u, v);
				return 1;
			}
			edgeList.push_back(std::make_tuple(u, v, w));
		}
	}

	// Build one message with all parameters and edges
	std::ostringstream oss;
	oss << seed << "\n";
	int directed = 1; // Always directed graph
	oss << directed << "\n" << vertices << "\n" << edges << "\n" << max_weight << "\n";
	for (auto& e : edgeList) {
	    oss << std::get<0>(e) << " " << std::get<1>(e) << " " << std::get<2>(e) << "\n";
	}
	std::string graphData = oss.str();
	if (send(sockfd, graphData.c_str(), graphData.size(), 0) == -1) {
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
