#ifndef CLIQUE_COUNT_ALGORITHM_HPP
#define CLIQUE_COUNT_ALGORITHM_HPP

#include "GraphAlgorithm.hpp"

class CliqueCountAlgorithm : public GraphAlgorithm {
public:
    std::string run(const Graph::Graph& graph) override;
};

#endif // CLIQUE_COUNT_ALGORITHM_HPP
