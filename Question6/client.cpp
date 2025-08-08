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
#include "Graph.hpp"

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

	// No need for hostname argument, always connect to 127.0.0.1
	const char* hostname = "127.0.0.1";

	// Open connection to server BEFORE reading input
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

	freeaddrinfo(servinfo); // all done with this structure

	// Now read input from user
	int vertices, edges;
	printf("Enter number of vertices: ");
	if (scanf("%d", &vertices) != 1 || vertices <= 0) {
	    fprintf(stderr, "Error: Number of vertices must be positive integer.\n");
	    exit(1);
	}
	printf("Enter number of edges: ");
	if (scanf("%d", &edges) != 1 || edges < 0) {
	    fprintf(stderr, "Error: Number of edges must be non-negative integer.\n");
	    exit(1);
	}

	std::vector<std::pair<int,int>> edgeList;
	for (int i = 0; i < edges; ++i) {
	    int u, v;
	    printf("Enter edge %d (u v): ", i+1);
	    if (scanf("%d %d", &u, &v) != 2 || u < 0 || u >= vertices || v < 0 || v >= vertices) {
	        fprintf(stderr, "Error: Invalid edge. Vertices must be between 0 and %d.\n", vertices-1);
	        exit(1);
	    }
	    if (u == v) {
	        fprintf(stderr, "Warning: Self-loops not allowed.\n");
	        exit(1);
	    }
	    bool duplicate = false;
	    for (auto& e : edgeList) {
	        if ((e.first == u && e.second == v) || (e.first == v && e.second == u)) duplicate = true;
	    }
	    if (duplicate) {
	        fprintf(stderr, "Warning: Duplicate edge (%d,%d) not allowed.\n", u, v);
	        exit(1);
	    }
	    edgeList.push_back({u,v});
	}

	// Build adjacency matrix text
	std::vector<std::vector<int>> adj(vertices, std::vector<int>(vertices, 0));
	for (auto& e : edgeList) {
	    adj[e.first][e.second]++;
	    adj[e.second][e.first]++;
	}
	char graphbuf[4096] = {0};
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
	    exit(1);
	}

	// Receive response from server
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}
	buf[numbytes] = '\0';
	printf("client: received '%s'\n",buf);

	close(sockfd);

	return 0;
}
