#include "Graph.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <queue>
#include <stack>

namespace Graph {

    Graph::Graph(int V, bool isDirected) : n(V), directed(isDirected) {
        if (V <= 0) {
            std::cerr << "Error: Number of vertices must be positive" << std::endl;
            n = 1;
        }
        adjMatrix.resize(n, std::vector<int>(n, 0));
    }

    Graph::~Graph() {}

    void Graph::addEdge(int u, int v, int weight) {
        // Validate vertices
        if (u < 0 || u >= n || v < 0 || v >= n) {
            std::cerr << "Error: Invalid vertex. Vertices must be between 0 and " 
                      << n - 1 << std::endl;
            return;
        }
        
        // Prevent self-loops
        if (u == v) {
            std::cerr << "Warning: Self-loops not allowed in this implementation" << std::endl;
            return;
        }
        
        // Check if edge already exists to prevent duplicates
        if (adjMatrix[u][v] != 0) {
            std::cerr << "Warning: Edge between " << u << " and " << v << " already exists" << std::endl;
            return;
        }
        
        // Add edge with weight
        adjMatrix[u][v] = weight;
        if (!directed) adjMatrix[v][u] = weight;
    }

    bool Graph::hasEdge(int u,int v) const{
        // Validate vertices
        if (u < 0 || u >= n || v < 0 || v >= n) {
            return false;
        }
        
        // Check if edge exists
        return adjMatrix[u][v] != 0;
    }

    int Graph::getEdgeWeight(int u, int v) const {
        // Validate vertices
        if (u < 0 || u >= n || v < 0 || v >= n) {
            return 0;
        }
        // Return the weight of the edge
        return adjMatrix[u][v];
    }

    int Graph::numOfVertices() const {
        return n;
    }

    bool Graph::isDirected() const {
        return directed;
    }

    void Graph::printGraph() const {
        std::cout << "Graph with " << n << " vertices:" << std::endl;
        for (int i = 0; i < n; ++i) {
            std::cout << "Vertex " << i << ": ";
            for (int j = 0; j < n; ++j) {
                if (adjMatrix[i][j] != 0) {
                    std::cout << "(" << j << ", weight: " << adjMatrix[i][j] << ") ";
                }
            }
            std::cout << std::endl;
        }
    }

    int Graph::getVertexDegree(int v) const {
        if (v < 0 || v >= n) return 0;
        int degree = 0;
        for (int j = 0; j < n; ++j) {
            if (adjMatrix[v][j] != 0) degree++;
        }
        return degree;
    }

    bool Graph::Connected() const {
        std::vector<bool> visited(n, false);
        int start = -1;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (adjMatrix[i][j] != 0) {
                    start = i;
                    break;
                }
            }
            if (start != -1) break;
        }
        if (start == -1 && n != 1) return false;
        if (start == -1 && n == 1) return true;
        std::queue<int> q;
        q.push(start);
        visited[start] = true;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v = 0; v < n; ++v) {
                if (adjMatrix[u][v] != 0 && !visited[v]) {
                    visited[v] = true;
                    q.push(v);
                }
            }
        }
        for (int i = 0; i < n; ++i) {
            bool hasEdge = false;
            for (int j = 0; j < n; ++j) {
                if (adjMatrix[i][j] != 0) { hasEdge = true; break; }
            }
            if (hasEdge && !visited[i]) return false;
        }
        return true;
    }

    std::vector<int> Graph::isEulerianCircuit() const {
        std::vector<int> eulerianCircuit;
        if (!Connected()) {
            std::cout << "No Eulerian Circuit! The graph is not connected." << std::endl;
            return eulerianCircuit;
        }
        for (int v = 0; v < numOfVertices(); v++) {
            if (getVertexDegree(v) % 2 != 0) {
                std::cout << "No Eulerian Circuit! Vertex " << v << " has an odd degree." << std::endl;
                return eulerianCircuit;
            }
        }
        std::vector<std::vector<int>> graphCopy = adjMatrix;
        std::stack<int> stack;
        int start = 0;
        for (int i = 0; i < numOfVertices(); ++i) {
            for (int j = 0; j < numOfVertices(); ++j) {
                if (graphCopy[i][j] != 0) { start = i; break; }
            }
            if (start != 0) break;
        }
        stack.push(start);
        while (!stack.empty()) {
            int u = stack.top();
            bool found = false;
            for (int v = 0; v < numOfVertices(); ++v) {
                if (graphCopy[u][v] != 0) {
                    graphCopy[u][v] = 0;
                    if (!isDirected()) graphCopy[v][u] = 0;
                    stack.push(v);
                    found = true;
                    break;
                }
            }
            if (!found) {
                eulerianCircuit.push_back(u);
                stack.pop();
            }
        }
        std::reverse(eulerianCircuit.begin(), eulerianCircuit.end());
        return eulerianCircuit;
    }

}
