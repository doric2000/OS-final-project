// main.cpp - Example usage of the Graph class
#include "Graph.hpp"
#include <iostream>
#include <vector>

int main() {
    // Create a graph with 5 vertices
    Graph::Graph g(5);

    // Add edges to make the graph Eulerian
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(4, 0); // This is a simple cycle, all degrees = 2 (even)

    // Print the graph's adjacency list
    g.printGraph();

    // Print the degree of each vertex
    for (int v = 0; v < g.numOfVertices(); ++v) {
        std::cout << "Degree of vertex " << v << ": " << g.getVertexDegree(v) << std::endl;
    }

    // Find and print Eulerian circuit if exists
    std::vector<int> circuit = g.isEulerianCircuit();
    if (!circuit.empty()) {
        std::cout << "Eulerian Circuit: ";
        for (size_t i = 0; i < circuit.size(); ++i) {
            std::cout << circuit[i];
            if (i + 1 < circuit.size()) std::cout << " -> ";
        }
        std::cout << std::endl;
    }
    return 0;
}