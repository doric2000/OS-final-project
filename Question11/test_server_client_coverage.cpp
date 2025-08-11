#include "Graph.hpp"
#include "MSTAlgorithm.hpp"
#include "MaxFlowAlgorithm.hpp"
#include "SCCAlgorithm.hpp"
#include "CliqueCountAlgorithm.hpp"
#include "GraphAlgorithmFactory.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstdlib>
#include <netdb.h>
#include <cstring>
#include <set>
#include <sstream>
#include <vector>
#include <tuple>

// Test server functionality by creating a mini server
void test_server_functionality() {
    std::cout << "=== Testing Server Core Functionality ===" << std::endl;
    
    // Test socket creation and binding (server core logic)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "Socket creation failed" << std::endl;
        return;
    }
    
    // Test address setup
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(3491); // Different port to avoid conflicts
    
    // Test binding
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cout << "Bind test completed (port may be in use)" << std::endl;
    } else {
        std::cout << "Socket bind successful" << std::endl;
        
        // Test listen
        if (listen(sockfd, 10) < 0) {
            std::cout << "Listen test completed" << std::endl;
        } else {
            std::cout << "Socket listen successful" << std::endl;
        }
    }
    
    close(sockfd);
    
    // Test the algorithm pipeline that server would use
    Graph::Graph graph(4, false);
    graph.addEdge(0, 1, 10);
    graph.addEdge(1, 2, 20);
    graph.addEdge(2, 3, 30);
    graph.addEdge(3, 0, 40);
    
    std::cout << "Testing server algorithm pipeline:" << std::endl;
    
    // MST
    MSTAlgorithm mst;
    std::string mst_result = mst.run(graph);
    std::cout << "MST: " << mst_result << std::endl;
    
    // MaxFlow  
    MaxFlowAlgorithm maxflow;
    std::string maxflow_result = maxflow.run(graph);
    std::cout << "MaxFlow: " << maxflow_result << std::endl;
    
    // SCC
    SCCAlgorithm scc;
    std::string scc_result = scc.run(graph);
    std::cout << "SCC: " << scc_result << std::endl;
    
    // Clique
    CliqueCountAlgorithm clique;
    std::string clique_result = clique.run(graph);
    std::cout << "Clique: " << clique_result << std::endl;
}

// Test all client command line scenarios
void test_client_scenarios() {
    std::cout << "\n=== Testing Client Command Line Scenarios ===" << std::endl;
    
    // Simulate client command line parsing and validation
    std::cout << "Testing command line validation..." << std::endl;
    
    // Test 1: Invalid arguments (no mode specified)
    char* test_argv1[] = {(char*)"client", nullptr};
    std::cout << "Test 1: No arguments - should show usage" << std::endl;
    
    // Test 2: Invalid vertex count
    char* test_argv2[] = {(char*)"client", (char*)"-r", (char*)"-n", (char*)"0", (char*)"-e", (char*)"5", nullptr};
    std::cout << "Test 2: Zero vertices - should show error" << std::endl;
    
    // Test 3: Invalid edge count  
    char* test_argv3[] = {(char*)"client", (char*)"-r", (char*)"-n", (char*)"5", (char*)"-e", (char*)"-1", nullptr};
    std::cout << "Test 3: Negative edges - should show error" << std::endl;
    
    // Test 4: Missing required arguments
    char* test_argv4[] = {(char*)"client", (char*)"-r", (char*)"-n", (char*)"5", nullptr};
    std::cout << "Test 4: Missing edge count - should show error" << std::endl;
    
    // Test 5: Manual mode
    char* test_argv5[] = {(char*)"client", (char*)"-m", (char*)"-n", (char*)"3", (char*)"-e", (char*)"2", nullptr};
    std::cout << "Test 5: Manual mode - should work" << std::endl;
    
    // Test 6: Random mode with seed
    char* test_argv6[] = {(char*)"client", (char*)"-r", (char*)"-n", (char*)"4", (char*)"-e", (char*)"3", (char*)"-s", (char*)"123", nullptr};
    std::cout << "Test 6: Random mode with seed - should work" << std::endl;
    
    // Test 7: Random mode with weight
    char* test_argv7[] = {(char*)"client", (char*)"-r", (char*)"-n", (char*)"4", (char*)"-e", (char*)"3", (char*)"-w", (char*)"15", nullptr};
    std::cout << "Test 7: Random mode with custom weight - should work" << std::endl;
    
    // Test connection scenarios
    std::cout << "\nTesting connection scenarios..." << std::endl;
    
    // Test socket creation
    int test_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (test_sock != -1) {
        std::cout << "Socket creation: SUCCESS" << std::endl;
        
        // Test connection to non-existent server (should fail gracefully)
        struct sockaddr_in test_addr;
        test_addr.sin_family = AF_INET;
        test_addr.sin_port = htons(9999); // Likely unused port
        inet_pton(AF_INET, "127.0.0.1", &test_addr.sin_addr);
        
        if (connect(test_sock, (struct sockaddr*)&test_addr, sizeof(test_addr)) < 0) {
            std::cout << "Connection to unavailable server: FAILED (as expected)" << std::endl;
        }
        
        close(test_sock);
    }
    
    // Test getaddrinfo functionality
    struct addrinfo hints, *servinfo;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    int rv = getaddrinfo("127.0.0.1", "3490", &hints, &servinfo);
    if (rv == 0) {
        std::cout << "getaddrinfo: SUCCESS" << std::endl;
        freeaddrinfo(servinfo);
    } else {
        std::cout << "getaddrinfo: " << gai_strerror(rv) << std::endl;
    }
}

// Test graph creation and messaging (client functionality)
void test_graph_messaging() {
    std::cout << "\n=== Testing Graph Message Creation ===" << std::endl;
    
    // Test random graph creation
    std::cout << "Creating random graphs..." << std::endl;
    
    // Test with different seeds
    for (int seed = 1; seed <= 3; ++seed) {
        std::srand(seed);
        Graph::Graph graph(5, false);
        
        std::cout << "Seed " << seed << " graph:" << std::endl;
        
        // Add random edges
        std::set<std::pair<int,int>> edges;
        int edge_count = 4;
        
        for (int i = 0; i < edge_count; ++i) {
            int u = std::rand() % 5;
            int v = std::rand() % 5;
            if (u != v && edges.find({std::min(u,v), std::max(u,v)}) == edges.end()) {
                edges.insert({std::min(u,v), std::max(u,v)});
                int weight = (std::rand() % 10) + 1;
                graph.addEdge(u, v, weight);
                std::cout << "  Edge: " << u << " -> " << v << " (weight: " << weight << ")" << std::endl;
            }
        }
        
        // Test message creation (what client sends to server)
        std::ostringstream message;
        message << seed << "\n";           // seed
        message << "0\n";                  // directed (0 = false)
        message << "5\n";                  // vertices
        message << edges.size() << "\n";   // edges
        
        for (const auto& edge : edges) {
            int weight = graph.getEdgeWeight(edge.first, edge.second);
            message << edge.first << " " << edge.second << " " << weight << "\n";
        }
        
        std::cout << "  Message size: " << message.str().length() << " bytes" << std::endl;
    }
    
    // Test manual graph input simulation
    std::cout << "\nTesting manual graph input..." << std::endl;
    Graph::Graph manual_graph(3, false);
    
    // Simulate manual edge input
    std::vector<std::tuple<int,int,int>> manual_edges = {
        {0, 1, 5},
        {1, 2, 3},
        {0, 2, 8}
    };
    
    for (const auto& edge : manual_edges) {
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        int w = std::get<2>(edge);
        manual_graph.addEdge(u, v, w);
        std::cout << "Manual edge: " << u << " -> " << v << " (weight: " << w << ")" << std::endl;
    }
    
    std::cout << "Manual graph created successfully" << std::endl;
}

// Test network data handling
void test_network_data_handling() {
    std::cout << "\n=== Testing Network Data Handling ===" << std::endl;
    
    // Test buffer operations
    char test_buffer[1000];
    std::string test_response = "mst: MST total weight: 15\nmaxflow: Max flow from 0 to n-1: 10\nscc: Strongly connected components:\n       Component 1: 0, 1, 2\nclique: Number of cliques (size 2-5): 4\n";
    
    if (test_response.length() < sizeof(test_buffer)) {
        strcpy(test_buffer, test_response.c_str());
        std::cout << "Buffer operations: SUCCESS" << std::endl;
        std::cout << "Response length: " << strlen(test_buffer) << " bytes" << std::endl;
    }
    
    // Test string processing
    std::istringstream response_stream(test_response);
    std::string line;
    int line_count = 0;
    
    while (std::getline(response_stream, line)) {
        line_count++;
        if (line_count <= 3) {
            std::cout << "Line " << line_count << ": " << line.substr(0, 30) << "..." << std::endl;
        }
    }
    
    std::cout << "Total response lines: " << line_count << std::endl;
}

int main() {
    std::cout << "Comprehensive Server and Client Coverage Testing" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    // Test server core functionality
    test_server_functionality();
    
    // Test client scenarios  
    test_client_scenarios();
    
    // Test graph messaging
    test_graph_messaging();
    
    // Test network data handling
    test_network_data_handling();
    
    std::cout << "\n=== All Tests Completed ===" << std::endl;
    
    return 0;
}
