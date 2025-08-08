CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g

TARGET = main
SRCS = Graph.cpp main.cpp
OBJS = Graph.o main.o

# Coverage flags
COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage

# Gprof flags
GPROF_FLAGS = -pg

all: $(TARGET) server client

server: server.o Graph.o
	$(CXX) $(CXXFLAGS) $^ -o $@

server.o: server.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

client: client.o Graph.o
	$(CXX) $(CXXFLAGS) $^ -o $@

client.o: client.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# coverage: clean
# 	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) $(SRCS) -o $(TARGET)

# gprof: clean
# 	$(CXX) $(CXXFLAGS) $(GPROF_FLAGS) $(SRCS) -o $(TARGET)

# run: $(TARGET)
# 	./$(TARGET)

# VALGRIND_ARGS = -v 1000 -e 5000 -s 42

# valgrind: $(TARGET)
# 	valgrind --leak-check=full ./$(TARGET) $(VALGRIND_ARGS) > valgrind_report.txt 2>&1

# callgrind: $(TARGET)
# 	valgrind --tool=callgrind --simulate-cache=yes ./$(TARGET) $(VALGRIND_ARGS)

clean:
	rm -f $(TARGET) *.o server client

# report:
# 	gcov *.cpp

.PHONY: all clean coverage gprof run valgrind callgrind report
