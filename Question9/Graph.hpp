#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>

namespace Graph {
	
    /**
     * @brief Graph class - represents a graph (directed or undirected) using an adjacency matrix
     * @details This class implements a graph data structure where vertices are numbered from 0 to V-1
     *          and edges are stored with weights in an adjacency matrix. If the graph is undirected,
     *          the edge weight is mirrored across the diagonal. A weight of 0 indicates no edge.
     */
    class Graph{
        private:
            int n; ///< Number of vertices in the graph
            bool directed; ///< Flag indicating if the graph is directed
            
            std::vector<std::vector<int>> adjMatrix; ///< Adjacency matrix representation - stores weights of edges

        public:
            /**
             * @brief Graph constructor - creates a graph with specified number of vertices
             * @param numVertices number of vertices in the graph
             * @param isDirected flag indicating if the graph is directed (default is false - undirected graph)
             */
            Graph(int numVertices, bool isDirected = false);

            /**
             * @brief Add an edge between two vertices with an optional weight
             * @param u first vertex (source)
             * @param v second vertex (destination)
             * @param weight weight of the edge (default is 1)
             */
            void addEdge(int u, int v, int weight = 1);

            /**
             * @brief Get the weight of the edge between two vertices
             * @param u first vertex
             * @param v second vertex
             * @return int weight of the edge, or 0 if no edge exists
             */
            int getEdgeWeight(int u, int v) const;

            /**
             * @brief Get the number of vertices in the graph
             * @return int number of vertices
             */
            int numOfVertices() const;

            /**
             * @brief Check if the graph is directed
             * @return true if the graph is directed, false if undirected
             */
            bool isDirected() const;

            /**
             * @brief Check if an edge exists between two vertices
             * @param u first vertex
             * @param v second vertex
             * @return true if edge exists, false otherwise
             */
            bool hasEdge(int u, int v) const;

            /**
             * @brief Print the adjacency matrix and all edges with weights
             */
            void printGraph() const;

            /**
             * @brief Get the degree of a vertex
             * @param v vertex index
             * @return int degree of the vertex
             */
            int getVertexDegree(int v) const;

            /**
             * @brief Check if the graph is connected
             * @return true if connected, false otherwise
             */
            bool Connected() const;

            /**
             * @brief Find an Eulerian circuit if it exists
             * @return vector<int> containing the circuit, empty if none exists
             */
            std::vector<int> isEulerianCircuit() const;

            /**
             * @brief Graph destructor - cleans up allocated memory
             */
            ~Graph();
    };    
    
}

#endif