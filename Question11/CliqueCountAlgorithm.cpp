#include "CliqueCountAlgorithm.hpp"
#include <vector>
#include <string>

std::string CliqueCountAlgorithm::run(const Graph::Graph& graph) {
    int n = graph.numOfVertices();
    int count = 0;
    for (int mask = 1; mask < (1 << n); ++mask) {
        std::vector<int> nodes;
        for (int i = 0; i < n; ++i) if (mask & (1 << i)) nodes.push_back(i);
        if (nodes.size() < 2 || nodes.size() > 5) continue;
        bool isClique = true;
        for (size_t i = 0; i < nodes.size(); ++i)
            for (size_t j = i + 1; j < nodes.size(); ++j)
                if (!graph.hasEdge(nodes[i], nodes[j])) isClique = false;
        if (isClique) ++count;
    }
    return "Number of cliques (size 2-5): " + std::to_string(count);
}
