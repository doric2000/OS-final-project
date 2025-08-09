#ifndef SCC_ALGORITHM_HPP
#define SCC_ALGORITHM_HPP

#include "GraphAlgorithm.hpp"
#include <vector>

class SCCAlgorithm : public GraphAlgorithm {
public:
    std::string run(const Graph::Graph& graph) override;
private:
    void dfs(const Graph::Graph& g, int v, std::vector<bool>& visited, std::vector<int>& order) const;
    void dfsRev(const Graph::Graph& g, int v, std::vector<bool>& visited, std::vector<int>& component) const;
};

#endif // SCC_ALGORITHM_HPP
