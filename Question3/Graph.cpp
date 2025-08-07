#include "Graph.hpp"
#include <iostream>
#include <algorithm>
#include <queue>
#include <stack>

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

    bool Graph::Connected() const {
        std::vector<bool> visited(NUM_OF_VERTICES, false);

        // Find a vertex with degree > 0 to start BFS
        int start = -1;
        for (int i = 0; i < NUM_OF_VERTICES; ++i) {
            if (!adjList[i].empty()) {
                start = i;
                break;
            }
        }
        if (start == -1) {
            // No edges in the graph, treat as connected
            return true;
        }

        // BFS
        std::queue<int> q;
        q.push(start);
        visited[start] = true;

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (int v : adjList[u]) {
                if (!visited[v]) {
                    visited[v] = true;
                    q.push(v);
                }
            }
        }

        // Check that all vertices with degree > 0 were visited
        for (int i = 0; i < NUM_OF_VERTICES; ++i) {
            if (!adjList[i].empty() && !visited[i]) {
                return false;
            }
        }
        return true;
    }

    std::vector<int> Graph::isEulerianCircuit() const{
        std::vector<int> eulerianCircuit;
        if (!this->Connected()){
            std::cout << "No Eulerian Circuit! The graph is not connected." << std::endl;
            return eulerianCircuit;
        }
        for (int v = 0; v < numOfVertices(); v++)
        {
            if (this->getVertexDegree(v) %2 != 0){
                std::cout << "No Eulerian Circuit! Vertex " << v << " has an odd degree." << std::endl;
                return eulerianCircuit;
            }
        }

        std::vector<std::vector<int>> graphCopy = adjList; //deep copy 
        std::stack<int> stack;
        int start = 0;
        // Find a vertex with degree > 0 to start
        for (int i = 0; i < numOfVertices(); ++i) {
            if (!graphCopy[i].empty()) {
                start = i;
                break;
            }
        }
        stack.push(start);

        while(!stack.empty()){
            int u=stack.top();
            if (!graphCopy[u].empty()){ //u has neighboors
                int v = graphCopy[u].back(); //take it last neighbor
                //remove edge uv as uv and as vu
                graphCopy[u].pop_back();
                auto it = std::find(graphCopy[v].begin(), graphCopy[v].end(), u);
                if (it != graphCopy[v].end()) {
                    graphCopy[v].erase(it);
                }
                stack.push(v);
            }
            else {
            // No more neighbors, add to circuit
            eulerianCircuit.push_back(u);
            stack.pop();
            }

        }
        // The circuit is built in reverse order
        std::reverse(eulerianCircuit.begin(), eulerianCircuit.end());
        return eulerianCircuit;
    }

    int Graph::getVertexDegree(int v) const {
        // Return the number of neighbors (degree) of vertex v
        if (v < 0 || v >= numOfVertices()) {
            return 0;
        }
        return adjList[v].size();
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
