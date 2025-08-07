# Basic Makefile for building the Graph project
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g

# Target executable name
TARGET = graph_test

# Source files
SRCS = Graph.cpp main.cpp

# Header files (for dependency tracking)
HEADERS = Graph.hpp

all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)
