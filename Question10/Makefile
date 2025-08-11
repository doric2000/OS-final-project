CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g -pthread

# All source files for server and client
SRCS = Graph.cpp MSTAlgorithm.cpp MaxFlowAlgorithm.cpp SCCAlgorithm.cpp CliqueCountAlgorithm.cpp GraphAlgorithmFactory.cpp
OBJS = Graph.o MSTAlgorithm.o MaxFlowAlgorithm.o SCCAlgorithm.o CliqueCountAlgorithm.o

all: server client

server: server.o $(OBJS) GraphAlgorithmFactory.o
	$(CXX) $(CXXFLAGS) $^ -o $@

server.o: server.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

GraphAlgorithmFactory.o: GraphAlgorithmFactory.cpp GraphAlgorithmFactory.hpp
	$(CXX) $(CXXFLAGS) -c GraphAlgorithmFactory.cpp -o GraphAlgorithmFactory.o

client: client.o $(OBJS) GraphAlgorithmFactory.o
	$(CXX) $(CXXFLAGS) $^ -o $@

client.o: client.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o server client

# Valgrind targets for server
valgrind-memcheck-server: server
	mkdir -p results
	timeout 10s valgrind --leak-check=full --show-leak-kinds=all ./server 2> results/memcheck_server.txt || true

valgrind-helgrind-server: server
	mkdir -p results
	timeout 10s valgrind --tool=helgrind ./server 2> results/helgrind_server.txt || true

valgrind-cg-server: server
	mkdir -p results
	timeout 10s valgrind --tool=callgrind ./server 2> results/cg_server.txt || true

# Valgrind targets for client (with test parameters)
valgrind-memcheck-client: client server
	mkdir -p results
	./server & SERVER_PID=$$!; sleep 1; valgrind --leak-check=full --show-leak-kinds=all ./client -r -n 5 -e 7 2> results/memcheck_client.txt; kill $$SERVER_PID 2>/dev/null || true

valgrind-helgrind-client: client server
	mkdir -p results
	./server & SERVER_PID=$$!; sleep 1; valgrind --tool=helgrind ./client -r -n 5 -e 7 2> results/helgrind_client.txt; kill $$SERVER_PID 2>/dev/null || true

valgrind-cg-client: client server
	mkdir -p results
	./server & SERVER_PID=$$!; sleep 1; valgrind --tool=callgrind ./client -r -n 5 -e 7 2> results/cg_client.txt; kill $$SERVER_PID 2>/dev/null || true

# Run all valgrind analyses
valgrind-all: all valgrind-memcheck-server valgrind-helgrind-server valgrind-cg-server valgrind-memcheck-client valgrind-helgrind-client valgrind-cg-client

.PHONY: all clean valgrind-memcheck-server valgrind-helgrind-server valgrind-cg-server valgrind-memcheck-client valgrind-helgrind-client valgrind-cg-client valgrind-all
