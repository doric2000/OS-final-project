#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>

namespace Graph {
	
    /**
     * @brief Graph class - represents an undirected graph using adjacency list
     * @details This class implements a graph data structure where vertices are numbered from 0 to V-1
     *          and edges are stored as adjacency lists for efficient neighbor access
     */
    class Graph{
        private:
            int NUM_OF_VERTICES; ///< Number of vertices in the graph
            
            std::vector<std::vector<int>> adjList; ///< Adjacency list representation - each vector contains neighbors of vertex i

        public:
            /**
             * @brief Graph constructor - creates a graph with specified number of vertices
             * @param V number of vertices in the graph
             */
            explicit Graph(int V);

            /**
             * @brief Add an undirected edge between two vertices
             * @param u first vertex (source)
             * @param v second vertex (destination)
             */
            void addEdge(int u, int v);

            /**
             * @brief Get the number of vertices in the graph
             * @return int number of vertices
             */
            int numOfVertices() const;

            /**
             * @brief Get the adjacency list representation of the graph
             * @return const std::vector<std::vector<int>>& reference to the adjacency list
             */
            const std::vector<std::vector<int>>& getAdjList() const;
            
            /**
             * @brief Print the graph structure to console
             */
            void printGraph() const;

             /**
             * @brief Check if an edge exists between two vertices
             * @param u first vertex
             * @param v second vertex
             * @return true if edge exists, false otherwise
             */
            bool hasEdge(int u, int v) const;

            /**
             * @brief Graph destructor - cleans up allocated memory
             */
            ~Graph();
    };    
    
}

#endif