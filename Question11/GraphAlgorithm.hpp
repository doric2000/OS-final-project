#ifndef GRAPH_ALGORITHM_HPP
#define GRAPH_ALGORITHM_HPP

#include "Graph.hpp"
#include <string>

class GraphAlgorithm {
public:
    virtual ~GraphAlgorithm() {}
    virtual std::string run(const Graph::Graph& graph) = 0;
};

#endif // GRAPH_ALGORITHM_HPP
