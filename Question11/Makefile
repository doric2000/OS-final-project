# Simple Makefile for building client-server graph algorithms application
# Usage:
#   make          - Build both server and client
#   make server   - Build only the server
#   make client   - Build only the client  
#   make clean    - Remove all compiled files

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g -pthread

# Source files needed for both server and client
ALGORITHM_SOURCES = Graph.cpp MSTAlgorithm.cpp MaxFlowAlgorithm.cpp SCCAlgorithm.cpp CliqueCountAlgorithm.cpp GraphAlgorithmFactory.cpp

# Object files (compiled .cpp files)
ALGORITHM_OBJECTS = Graph.o MSTAlgorithm.o MaxFlowAlgorithm.o SCCAlgorithm.o CliqueCountAlgorithm.o GraphAlgorithmFactory.o

# Default target: build both server and client
all: server client

# Build the server executable
server: server.o $(ALGORITHM_OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Build the client executable  
client: client.o $(ALGORITHM_OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile server.cpp to server.o
server.o: server.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile client.cpp to client.o
client.o: client.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# General rule: compile any .cpp file to .o file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up compiled files
clean:
	rm -f *.o server client

# Tell make that these targets don't create files with the same name
.PHONY: all clean
