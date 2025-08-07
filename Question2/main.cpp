// main.cpp - Example usage of the Graph class
#include "Graph.hpp"
#include <iostream>

int main() {
    // Create a graph with 5 vertices
    Graph::Graph g(5);

    // Add some edges
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);
    g.addEdge(3, 4);

    // Try to add duplicate and invalid edges
    g.addEdge(0, 1); // Duplicate edge
    g.addEdge(0, 5); // Invalid vertex
    g.addEdge(2, 2); // Self-loop

    // Check if certain edges exist
    std::cout << "Edge between 0 and 1: " << (g.hasEdge(0, 1) ? "Exists" : "Does not exist") << std::endl;
    std::cout << "Edge between 1 and 3: " << (g.hasEdge(1, 3) ? "Exists" : "Does not exist") << std::endl;

    // Print the graph's adjacency list
    g.printGraph();

    return 0;
}
