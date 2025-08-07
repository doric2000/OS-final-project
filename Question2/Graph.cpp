#include "Graph.hpp"
#include <iostream>
#include <algorithm>

namespace Graph {

    Graph::Graph(int V) : NUM_OF_VERTICES(V) {
        if (V <= 0) {
            std::cerr << "Error: Number of vertices must be positive" << std::endl;
            NUM_OF_VERTICES = 1;
        }
        adjList.resize(NUM_OF_VERTICES); //change the size of the vector to V
    }

    Graph::~Graph() {
        //vectors clean themselves automatically
    }

    void Graph::addEdge(int u, int v) {
        // Validate vertices
        if (u < 0 || u >= NUM_OF_VERTICES || v < 0 || v >= NUM_OF_VERTICES) {
            std::cerr << "Error: Invalid vertex. Vertices must be between 0 and " 
                      << NUM_OF_VERTICES - 1 << std::endl;
            return;
        }
        
        // Prevent self-loops
        if (u == v) {
            std::cerr << "Warning: Self-loops not allowed in this implementation" << std::endl;
            return;
        }
        
        // Check if edge already exists to prevent duplicates
        if (hasEdge(u, v)) {
            std::cerr << "Warning: Edge between " << u << " and " << v << " already exists" << std::endl;
            return;
        }
        
        // Add edge in both directions (undirected graph)
        adjList[u].push_back(v);
        adjList[v].push_back(u);
    }

    bool Graph::hasEdge(int u,int v) const{
        // Validate vertices
        if (u < 0 || u >= numOfVertices() || v < 0 || v >= NUM_OF_VERTICES) {
            return false;
        }
        
        // Check if v is in u's adjacency list
        return std::find(adjList[u].begin(), adjList[u].end(), v) != adjList[u].end();
    }

    int Graph::numOfVertices() const {
        return NUM_OF_VERTICES;
    }

    const std::vector<std::vector<int>>& Graph::getAdjList() const {
        return adjList;
    }

    void Graph::printGraph() const {
        std::cout << "Graph with " << NUM_OF_VERTICES << " vertices:" << std::endl;
        for (int i = 0; i < NUM_OF_VERTICES; ++i) {
            std::cout << "Vertex " << i << ": ";
            if (adjList[i].empty()) {
                std::cout << "(no neighbors)";
            } else {
                for (size_t j = 0; j < adjList[i].size(); ++j) {
                    std::cout << adjList[i][j];
                    if (j < adjList[i].size() - 1) {
                        std::cout << ", ";
                    }
                }
            }
            std::cout << std::endl;
        }
        //std::cout << "Total edges: " << getNumEdges() << std::endl;
    }

} 
