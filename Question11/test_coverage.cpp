#include "Graph.hpp"
#include "MSTAlgorithm.hpp"
#include "MaxFlowAlgorithm.hpp"
#include "SCCAlgorithm.hpp"
#include "CliqueCountAlgorithm.hpp"
#include "GraphAlgorithmFactory.hpp"
#include <iostream>
#include <memory>

int main() {
    std::cout << "Testing all algorithms for comprehensive coverage..." << std::endl;
    
    // Test 1: Small graph
    std::cout << "=== Test 1: Small Graph ===" << std::endl;
    Graph::Graph g1(3);
    g1.addEdge(0, 1, 5);
    g1.addEdge(1, 2, 3);
    g1.addEdge(2, 0, 2);
    
    // Test MST Algorithm
    std::cout << "Testing MST Algorithm..." << std::endl;
    MSTAlgorithm mst;
    std::string mst_result = mst.run(g1);
    std::cout << "MST Result: " << mst_result << std::endl;
    
    // Test Max Flow Algorithm
    std::cout << "Testing Max Flow Algorithm..." << std::endl;
    MaxFlowAlgorithm maxflow;
    std::string flow_result = maxflow.run(g1);
    std::cout << "Max Flow Result: " << flow_result << std::endl;
    
    // Test SCC Algorithm
    std::cout << "Testing SCC Algorithm..." << std::endl;
    SCCAlgorithm scc;
    std::string scc_result = scc.run(g1);
    std::cout << "SCC Result: " << scc_result << std::endl;
    
    // Test Clique Count Algorithm
    std::cout << "Testing Clique Count Algorithm..." << std::endl;
    CliqueCountAlgorithm clique;
    std::string clique_result = clique.run(g1);
    std::cout << "Clique Result: " << clique_result << std::endl;
    
    // Test 2: Medium graph
    std::cout << "\n=== Test 2: Medium Graph ===" << std::endl;
    Graph::Graph g2(5);
    for (int i = 0; i < 5; i++) {
        for (int j = i + 1; j < 5; j++) {
            g2.addEdge(i, j, (i + j) % 10 + 1);
        }
    }
    
    // Test all algorithms with medium graph
    std::cout << "Testing with medium graph..." << std::endl;
    MSTAlgorithm mst2;
    MaxFlowAlgorithm maxflow2;
    SCCAlgorithm scc2;
    CliqueCountAlgorithm clique2;
    
    std::cout << "MST: " << mst2.run(g2) << std::endl;
    std::cout << "Max Flow: " << maxflow2.run(g2) << std::endl;
    std::cout << "SCC: " << scc2.run(g2) << std::endl;
    std::cout << "Clique: " << clique2.run(g2) << std::endl;
    
    // Test 3: Empty graph
    std::cout << "\n=== Test 3: Empty Graph ===" << std::endl;
    Graph::Graph g3(2);
    std::cout << "Testing with empty graph..." << std::endl;
    MSTAlgorithm mst3;
    MaxFlowAlgorithm maxflow3;
    SCCAlgorithm scc3;
    CliqueCountAlgorithm clique3;
    
    std::cout << "MST: " << mst3.run(g3) << std::endl;
    std::cout << "Max Flow: " << maxflow3.run(g3) << std::endl;
    std::cout << "SCC: " << scc3.run(g3) << std::endl;
    std::cout << "Clique: " << clique3.run(g3) << std::endl;
    
    // Test 4: Single vertex
    std::cout << "\n=== Test 4: Single Vertex ===" << std::endl;
    Graph::Graph g4(1);
    std::cout << "Testing with single vertex..." << std::endl;
    MSTAlgorithm mst4;
    MaxFlowAlgorithm maxflow4;
    SCCAlgorithm scc4;
    CliqueCountAlgorithm clique4;
    
    std::cout << "MST: " << mst4.run(g4) << std::endl;
    std::cout << "Max Flow: " << maxflow4.run(g4) << std::endl;
    std::cout << "SCC: " << scc4.run(g4) << std::endl;
    std::cout << "Clique: " << clique4.run(g4) << std::endl;
    
    // Test Factory
    std::cout << "\n=== Test 5: Factory Pattern ===" << std::endl;
    Graph::Graph g_factory(3);
    g_factory.addEdge(0, 1, 5);
    g_factory.addEdge(1, 2, 3);
    g_factory.addEdge(2, 0, 2);
    
    GraphAlgorithm* mst_factory = GraphAlgorithmFactory::create("mst");
    GraphAlgorithm* flow_factory = GraphAlgorithmFactory::create("maxflow");
    GraphAlgorithm* scc_factory = GraphAlgorithmFactory::create("scc");
    GraphAlgorithm* clique_factory = GraphAlgorithmFactory::create("clique");
    
    if (mst_factory) {
        std::cout << "Factory MST: " << mst_factory->run(g_factory) << std::endl;
        delete mst_factory;
    }
    if (flow_factory) {
        std::cout << "Factory MaxFlow: " << flow_factory->run(g_factory) << std::endl;
        delete flow_factory;
    }
    if (scc_factory) {
        std::cout << "Factory SCC: " << scc_factory->run(g_factory) << std::endl;
        delete scc_factory;
    }
    if (clique_factory) {
        std::cout << "Factory Clique: " << clique_factory->run(g_factory) << std::endl;
        delete clique_factory;
    }
    
    // Test unknown algorithm type
    GraphAlgorithm* unknown = GraphAlgorithmFactory::create("unknown");
    if (unknown == nullptr) {
        std::cout << "Factory correctly returned nullptr for unknown algorithm" << std::endl;
    }
    
    // Test Graph methods comprehensively
    std::cout << "\n=== Test 6: Graph Methods ===" << std::endl;
    Graph::Graph g5(4);
    g5.addEdge(0, 1, 10);
    g5.addEdge(1, 2, 20);
    g5.addEdge(2, 3, 30);
    g5.addEdge(3, 0, 40);
    
    std::cout << "Graph vertices: " << g5.numOfVertices() << std::endl;
    std::cout << "Graph is directed: " << g5.isDirected() << std::endl;
    std::cout << "Has edge (0,1): " << g5.hasEdge(0, 1) << std::endl;
    std::cout << "Edge weight (0,1): " << g5.getEdgeWeight(0, 1) << std::endl;
    std::cout << "Vertex 0 degree: " << g5.getVertexDegree(0) << std::endl;
    std::cout << "Graph connected: " << g5.Connected() << std::endl;
    
    // Test different edge cases for better coverage
    std::cout << "\n=== Test 7: Cycle Graph ===" << std::endl;
    Graph::Graph g6(6);
    g6.addEdge(0, 1, 1);
    g6.addEdge(1, 2, 1);
    g6.addEdge(2, 3, 1);
    g6.addEdge(3, 4, 1);
    g6.addEdge(4, 5, 1);
    g6.addEdge(5, 0, 1);
    
    std::cout << "Testing algorithms with cycle graph..." << std::endl;
    MSTAlgorithm mst6;
    MaxFlowAlgorithm maxflow6;
    SCCAlgorithm scc6;
    CliqueCountAlgorithm clique6;
    
    std::cout << "MST: " << mst6.run(g6) << std::endl;
    std::cout << "Max Flow: " << maxflow6.run(g6) << std::endl;
    std::cout << "SCC: " << scc6.run(g6) << std::endl;
    std::cout << "Clique: " << clique6.run(g6) << std::endl;
    
    // Test 8: Disconnected components
    std::cout << "\n=== Test 8: Disconnected Graph ===" << std::endl;
    Graph::Graph g7(6);
    // First component
    g7.addEdge(0, 1, 10);
    g7.addEdge(1, 2, 20);
    // Second component
    g7.addEdge(3, 4, 30);
    g7.addEdge(4, 5, 40);
    
    MSTAlgorithm mst7;
    MaxFlowAlgorithm maxflow7;
    SCCAlgorithm scc7;
    CliqueCountAlgorithm clique7;
    
    std::cout << "MST: " << mst7.run(g7) << std::endl;
    std::cout << "Max Flow: " << maxflow7.run(g7) << std::endl;
    std::cout << "SCC: " << scc7.run(g7) << std::endl;
    std::cout << "Clique: " << clique7.run(g7) << std::endl;
    
    std::cout << "\nAll algorithm tests completed!" << std::endl;
    return 0;
}
