#ifndef GRAPH_ALGORITHM_FACTORY_HPP
#define GRAPH_ALGORITHM_FACTORY_HPP

#include "MSTAlgorithm.hpp"
#include "MaxFlowAlgorithm.hpp"
#include "SCCAlgorithm.hpp"
#include "CliqueCountAlgorithm.hpp"
#include "GraphAlgorithm.hpp"
#include <string>

//factory :

class GraphAlgorithmFactory {
public:
    static GraphAlgorithm* create(const std::string& name) {
        if (name == "mst") return new MSTAlgorithm();
        if (name == "maxflow") return new MaxFlowAlgorithm();
        if (name == "scc") return new SCCAlgorithm();
        if (name == "clique") return new CliqueCountAlgorithm();
        // more algorithms here :D
        return nullptr;
    }
};

#endif // GRAPH_ALGORITHM_FACTORY_HPP
