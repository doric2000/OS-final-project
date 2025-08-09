#ifndef MAX_FLOW_ALGORITHM_HPP
#define MAX_FLOW_ALGORITHM_HPP

#include "GraphAlgorithm.hpp"
#include <vector>

class MaxFlowAlgorithm : public GraphAlgorithm {
public:
    std::string run(const Graph::Graph& graph) override;
};

#endif // MAX_FLOW_ALGORITHM_HPP
