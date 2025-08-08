#ifndef MST_ALGORITHM_HPP
#define MST_ALGORITHM_HPP

#include "GraphAlgorithm.hpp"

class MSTAlgorithm : public GraphAlgorithm {
public:
    std::string run(const Graph::Graph& graph) override;
};

#endif // MST_ALGORITHM_HPP
