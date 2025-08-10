#include "MaxFlowAlgorithm.hpp"
#include <limits.h>
#include <queue>
#include <vector>
#include <string>

std::string MaxFlowAlgorithm::run(const Graph::Graph& graph) {
    int n = graph.numOfVertices();
    std::vector<std::vector<int>> capacity(n, std::vector<int>(n, 0));
    for (int u = 0; u < n; ++u)
        for (int v = 0; v < n; ++v)
            capacity[u][v] = graph.getEdgeWeight(u, v);
    int flow = 0;
    std::vector<int> parent(n);
    auto bfs = [&](int s, int t) -> int {
        std::fill(parent.begin(), parent.end(), -1);
        parent[s] = -2;
        std::queue<std::pair<int, int>> q;
        q.push({s, INT_MAX});
        while (!q.empty()) {
            int u = q.front().first, f = q.front().second; q.pop();
            for (int v = 0; v < n; ++v) {
                if (parent[v] == -1 && capacity[u][v] > 0) {
                    parent[v] = u;
                    int new_f = std::min(f, capacity[u][v]);
                    if (v == t) return new_f;
                    q.push({v, new_f});
                }
            }
        }
        return 0;
    };
    int s = 0, t = n - 1;
    int new_flow;
    do {
        new_flow = bfs(s, t);
        if (new_flow) {
            flow += new_flow;
            int v = t;
            while (v != s) {
                int u = parent[v];
                capacity[u][v] -= new_flow;
                capacity[v][u] += new_flow;
                v = u;
            }
        }
    } while (new_flow);
    return "Max flow from 0 to n-1: " + std::to_string(flow);
}
