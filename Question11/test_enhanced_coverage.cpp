#include "Graph.hpp"
#include "MSTAlgorithm.hpp"
#include "MaxFlowAlgorithm.hpp"
#include "SCCAlgorithm.hpp"
#include "CliqueCountAlgorithm.hpp"
#include "GraphAlgorithmFactory.hpp"
#include <iostream>
#include <memory>

int main() {
    std::cout << "Testing all algorithms and edge cases for 80%+ coverage..." << std::endl;
    
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
    
    // Test 2: Error conditions for Graph
    std::cout << "\n=== Test 2: Error Conditions ===" << std::endl;
    
    // Invalid vertices in constructor (should trigger error path)
    Graph::Graph g_invalid(0);  // This should trigger n = 1 fallback
    Graph::Graph g_negative(-5);  // This should trigger n = 1 fallback
    
    // Invalid vertices in addEdge
    Graph::Graph g_error(3);
    g_error.addEdge(10, 1, 5);  // Invalid vertex u
    g_error.addEdge(0, 10, 5);  // Invalid vertex v
    g_error.addEdge(0, 0, 5);   // Self-loop
    g_error.addEdge(0, 1, 5);   // Valid edge
    g_error.addEdge(0, 1, 10);  // Duplicate edge
    
    // Test other Graph methods for edge cases
    std::cout << "Edge weight of non-existent edge: " << g_error.getEdgeWeight(0, 2) << std::endl;
    std::cout << "Has edge that doesn't exist: " << g_error.hasEdge(0, 2) << std::endl;
    std::cout << "Vertex degree: " << g_error.getVertexDegree(0) << std::endl;
    std::cout << "Is connected: " << g_error.Connected() << std::endl;
    
    // Test Eulerian circuit
    auto circuit = g_error.isEulerianCircuit();
    std::cout << "Eulerian circuit size: " << circuit.size() << std::endl;
    
    // Test printGraph method
    std::cout << "Printing graph structure:" << std::endl;
    g_error.printGraph();
    
    // Test 3: Medium graph with comprehensive tests
    std::cout << "\n=== Test 3: Medium Graph ===" << std::endl;
    Graph::Graph g2(5);
    for (int i = 0; i < 5; i++) {
        for (int j = i + 1; j < 5; j++) {
            g2.addEdge(i, j, (i + j) % 10 + 1);
        }
    }
    
    // Test all Graph methods
    std::cout << "Graph vertices: " << g2.numOfVertices() << std::endl;
    std::cout << "Graph is directed: " << g2.isDirected() << std::endl;
    std::cout << "Has edge (0,1): " << g2.hasEdge(0, 1) << std::endl;
    std::cout << "Edge weight (0,1): " << g2.getEdgeWeight(0, 1) << std::endl;
    std::cout << "Vertex 0 degree: " << g2.getVertexDegree(0) << std::endl;
    std::cout << "Graph connected: " << g2.Connected() << std::endl;
    g2.printGraph();
    
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
    
    // Test 4: Empty graph
    std::cout << "\n=== Test 4: Empty Graph ===" << std::endl;
    Graph::Graph g3(2);
    std::cout << "Testing with empty graph..." << std::endl;
    MSTAlgorithm mst3;
    MaxFlowAlgorithm maxflow3;
    SCCAlgorithm scc3;
    CliqueCountAlgorithm clique3;
    
    std::cout << "Graph connected (empty): " << g3.Connected() << std::endl;
    std::cout << "MST: " << mst3.run(g3) << std::endl;
    std::cout << "Max Flow: " << maxflow3.run(g3) << std::endl;
    std::cout << "SCC: " << scc3.run(g3) << std::endl;
    std::cout << "Clique: " << clique3.run(g3) << std::endl;
    
    // Test 5: Single vertex
    std::cout << "\n=== Test 5: Single Vertex ===" << std::endl;
    Graph::Graph g4(1);
    std::cout << "Testing with single vertex..." << std::endl;
    MSTAlgorithm mst4;
    MaxFlowAlgorithm maxflow4;
    SCCAlgorithm scc4;
    CliqueCountAlgorithm clique4;
    
    std::cout << "Single vertex connected: " << g4.Connected() << std::endl;
    std::cout << "MST: " << mst4.run(g4) << std::endl;
    std::cout << "Max Flow: " << maxflow4.run(g4) << std::endl;
    std::cout << "SCC: " << scc4.run(g4) << std::endl;
    std::cout << "Clique: " << clique4.run(g4) << std::endl;
    
    // Test 6: Factory Pattern (comprehensive)
    std::cout << "\n=== Test 6: Factory Pattern ===" << std::endl;
    Graph::Graph g_factory(3);
    g_factory.addEdge(0, 1, 5);
    g_factory.addEdge(1, 2, 3);
    g_factory.addEdge(2, 0, 2);
    
    // Test all algorithm types through factory
    std::vector<std::string> algorithm_types = {"mst", "maxflow", "scc", "clique", "unknown", "invalid", ""};
    
    for (const auto& algo_type : algorithm_types) {
        std::cout << "Testing factory with algorithm type: '" << algo_type << "'" << std::endl;
        GraphAlgorithm* algo = GraphAlgorithmFactory::create(algo_type);
        if (algo) {
            std::cout << "Factory result for " << algo_type << ": " << algo->run(g_factory) << std::endl;
            delete algo;
        } else {
            std::cout << "Factory correctly returned nullptr for: " << algo_type << std::endl;
        }
    }
    
    // Test 7: Directed graph
    std::cout << "\n=== Test 7: Directed Graph ===" << std::endl;
    Graph::Graph g_directed(4, true);  // Create directed graph
    g_directed.addEdge(0, 1, 10);
    g_directed.addEdge(1, 2, 20);
    g_directed.addEdge(2, 3, 30);
    g_directed.addEdge(3, 0, 40);
    
    std::cout << "Directed graph is directed: " << g_directed.isDirected() << std::endl;
    std::cout << "Directed graph connected: " << g_directed.Connected() << std::endl;
    std::cout << "Has edge (0,1): " << g_directed.hasEdge(0, 1) << std::endl;
    std::cout << "Has reverse edge (1,0): " << g_directed.hasEdge(1, 0) << std::endl;
    g_directed.printGraph();
    
    // Test algorithms on directed graph
    MSTAlgorithm mst_dir;
    MaxFlowAlgorithm maxflow_dir;
    SCCAlgorithm scc_dir;
    CliqueCountAlgorithm clique_dir;
    
    std::cout << "Directed MST: " << mst_dir.run(g_directed) << std::endl;
    std::cout << "Directed Max Flow: " << maxflow_dir.run(g_directed) << std::endl;
    std::cout << "Directed SCC: " << scc_dir.run(g_directed) << std::endl;
    std::cout << "Directed Clique: " << clique_dir.run(g_directed) << std::endl;
    
    // Test 8: Disconnected components
    std::cout << "\n=== Test 8: Disconnected Graph ===" << std::endl;
    Graph::Graph g7(6);
    // First component
    g7.addEdge(0, 1, 10);
    g7.addEdge(1, 2, 20);
    // Second component (isolated)
    g7.addEdge(3, 4, 30);
    g7.addEdge(4, 5, 40);
    // Node 2 and 5 are connected to their components but components are separate
    
    std::cout << "Disconnected graph connected: " << g7.Connected() << std::endl;
    g7.printGraph();
    
    MSTAlgorithm mst7;
    MaxFlowAlgorithm maxflow7;
    SCCAlgorithm scc7;
    CliqueCountAlgorithm clique7;
    
    std::cout << "Disconnected MST: " << mst7.run(g7) << std::endl;
    std::cout << "Disconnected Max Flow: " << maxflow7.run(g7) << std::endl;
    std::cout << "Disconnected SCC: " << scc7.run(g7) << std::endl;
    std::cout << "Disconnected Clique: " << clique7.run(g7) << std::endl;
    
    // Test 9: Stress test with larger graph
    std::cout << "\n=== Test 9: Stress Test ===" << std::endl;
    Graph::Graph g8(10);
    // Create a complex graph with various patterns
    for (int i = 0; i < 10; i++) {
        for (int j = i + 1; j < 10; j++) {
            if ((i + j) % 3 == 0) {  // Selective edges to create interesting patterns
                g8.addEdge(i, j, (i * j) % 20 + 1);
            }
        }
    }
    
    std::cout << "Large graph connected: " << g8.Connected() << std::endl;
    std::cout << "Large graph vertices: " << g8.numOfVertices() << std::endl;
    
    // Test Eulerian circuit on larger graph
    auto large_circuit = g8.isEulerianCircuit();
    std::cout << "Large graph Eulerian circuit size: " << large_circuit.size() << std::endl;
    
    // Run all algorithms
    MSTAlgorithm mst8;
    MaxFlowAlgorithm maxflow8;
    SCCAlgorithm scc8;
    CliqueCountAlgorithm clique8;
    
    std::cout << "Large MST: " << mst8.run(g8) << std::endl;
    std::cout << "Large Max Flow: " << maxflow8.run(g8) << std::endl;
    std::cout << "Large SCC: " << scc8.run(g8) << std::endl;
    std::cout << "Large Clique: " << clique8.run(g8) << std::endl;
    
    std::cout << "\nAll comprehensive algorithm tests completed!" << std::endl;
    return 0;
}
