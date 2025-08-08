// main.cpp - Example usage of the Graph class
#include "Graph.hpp"
#include <iostream>
#include <vector>
#include <getopt.h>
#include <random>

int main(int argc, char* argv[])  {
    int vertices = -1, edges = -1, seed = -1;
    int opt;
    bool v_flag = false, e_flag = false, s_flag = false;

    while ((opt = getopt(argc, argv, "v:e:s:")) != -1) {
        switch (opt) {
            case 'v': vertices = std::stoi(optarg); v_flag = true; break;
            case 'e': edges = std::stoi(optarg); e_flag = true; break;
            case 's': seed = std::stoi(optarg); s_flag = true; break;
            default:
                std::cerr << "Usage: " << argv[0] << " -v vertices -e edges -s seed\n";
                return 1;
        }
    }

    // If any flag is missing, print usage
    if (!v_flag || !e_flag || !s_flag) {
        std::cerr << "Usage: " << argv[0] << " -v vertices -e edges -s seed\n";
        return 1;
    }

    // If values are invalid, print error
    if (vertices <= 0 || edges < 0) {
        std::cerr << "Invalid parameters. use actual and positive numbers.\n";
        return 1;
    }

    // Create a graph with x vertices as user gave us.
    Graph::Graph g(vertices);

    //set random values:
    std::mt19937 rng(seed); //creating random numbers with the given seed to "Save" the random sequence.
    std::uniform_int_distribution<int> dist(0, vertices - 1); // creates a uniform range of random ints from '0-vertices-1'

    //add edges within the uniform range
    int added = 0;
    while (added < edges) {
        int u = dist(rng);
        int v = dist(rng);
        if (u != v && !g.hasEdge(u, v)) {
            g.addEdge(u, v);
            ++added;
        }
    }

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