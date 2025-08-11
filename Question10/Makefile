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
	valgrind --leak-check=full --show-leak-kinds=all ./server > valgrind_memcheck_server.log 2>&1

valgrind-helgrind-server: server
	valgrind --tool=helgrind ./server > valgrind_helgrind_server.log 2>&1

valgrind-callgrind-server: server
	valgrind --tool=callgrind ./server > valgrind_callgrind_server.log 2>&1

# Valgrind targets for client
valgrind-memcheck-client: client
	valgrind --leak-check=full --show-leak-kinds=all ./client > valgrind_memcheck_client.log 2>&1

valgrind-helgrind-client: client
	valgrind --tool=helgrind ./client > valgrind_helgrind_client.log 2>&1

valgrind-callgrind-client: client
	valgrind --tool=callgrind ./client > valgrind_callgrind_client.log 2>&1

.PHONY: all clean valgrind-memcheck-server valgrind-helgrind-server valgrind-callgrind-server valgrind-memcheck-client valgrind-helgrind-client valgrind-callgrind-client
