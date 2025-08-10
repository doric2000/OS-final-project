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
            if (!inMST[v]) {
                int w1 = graph.getEdgeWeight(u, v);
                int w2 = graph.getEdgeWeight(v, u);
                int w = 0;
                if (w1 > 0 && w2 > 0) w = std::min(w1, w2);
                else if (w1 > 0) w = w1;
                else if (w2 > 0) w = w2;
                if (w > 0 && w < minEdge[v]) minEdge[v] = w;
            }
        }
    }
    return "MST total weight: " + std::to_string(totalWeight);
}
