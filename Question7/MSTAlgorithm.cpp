#include "MSTAlgorithm.hpp"
#include <limits.h>
#include <vector>
#include <string>

std::string MSTAlgorithm::run(const Graph::Graph& graph) {
    int n = graph.numOfVertices();
    std::vector<bool> inMST(n, false);
    std::vector<int> minEdge(n, INT_MAX);
    minEdge[0] = 0;
    int totalWeight = 0;
    for (int i = 0; i < n; ++i) {
        int u = -1;
        for (int v = 0; v < n; ++v) {
            if (!inMST[v] && (u == -1 || minEdge[v] < minEdge[u])) u = v;
        }
        if (minEdge[u] == INT_MAX) return "Graph not connected";
        inMST[u] = true;
        totalWeight += minEdge[u];
        for (int v = 0; v < n; ++v) {
            int w = graph.getEdgeWeight(u, v);
            if (w > 0 && !inMST[v] && w < minEdge[v]) minEdge[v] = w;
        }
    }
    return "MST total weight: " + std::to_string(totalWeight);
}
