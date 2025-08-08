CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g

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

.PHONY: all clean
