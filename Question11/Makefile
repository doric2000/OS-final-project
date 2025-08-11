CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g -pthread
COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage -O0

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
	rm -f *.o server client test_coverage test_enhanced_coverage test_client_coverage test_server_coverage test_server_client_comprehensive test_server_client_coverage

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

# Code coverage targets
coverage: clean build-coverage run-coverage run-algorithm-tests generate-coverage-report

# Build everything with coverage flags
build-coverage:
	$(MAKE) clean
	$(MAKE) server-coverage client-coverage test-coverage test-enhanced-coverage

# Build server with coverage flags
server-coverage:
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -c server.cpp -o server.o
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -c Graph.cpp -o Graph.o
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -c MSTAlgorithm.cpp -o MSTAlgorithm.o
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -c MaxFlowAlgorithm.cpp -o MaxFlowAlgorithm.o
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -c SCCAlgorithm.cpp -o SCCAlgorithm.o
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -c CliqueCountAlgorithm.cpp -o CliqueCountAlgorithm.o
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -c GraphAlgorithmFactory.cpp -o GraphAlgorithmFactory.o
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) server.o $(OBJS) GraphAlgorithmFactory.o -o server -lgcov

# Build client with coverage flags
client-coverage:
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -c client.cpp -o client.o
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) client.o $(OBJS) GraphAlgorithmFactory.o -o client -lgcov

# Build comprehensive test with coverage flags
test-coverage:
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -c test_coverage.cpp -o test_coverage.o
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) test_coverage.o $(OBJS) GraphAlgorithmFactory.o -o test_coverage

# Build enhanced comprehensive test with coverage flags for maximum coverage
test-enhanced-coverage:
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -c test_enhanced_coverage.cpp -o test_enhanced_coverage.o
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) test_enhanced_coverage.o $(OBJS) GraphAlgorithmFactory.o -o test_enhanced_coverage

# Test programs for comprehensive coverage
test_client_coverage: test_client_coverage.o $(OBJS) GraphAlgorithmFactory.o
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) $^ -o $@

test_client_coverage.o: test_client_coverage.cpp
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -c $< -o $@

test_server_coverage: test_server_coverage.o $(OBJS) GraphAlgorithmFactory.o
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) $^ -o $@

test_server_coverage.o: test_server_coverage.cpp
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -c $< -o $@

test_server_client_comprehensive: test_server_client_comprehensive.o $(OBJS) GraphAlgorithmFactory.o
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) $^ -o $@

test_server_client_comprehensive.o: test_server_client_comprehensive.cpp
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -c $< -o $@

# Build all test programs
build-coverage-tests: test_client_coverage test_server_coverage test_server_client_comprehensive

# Coverage test targets
test_server_client_coverage: test_server_client_coverage.cpp
	$(CXX) $(COVERAGE_FLAGS) $(SRCS) test_server_client_coverage.cpp -o $@

# Run comprehensive algorithm testing for maximum coverage
run-algorithm-tests:
	@echo "Running comprehensive algorithm tests for maximum coverage..."
	./test_coverage
	@echo "Algorithm testing complete!"

# Run programs to generate coverage data with comprehensive testing
run-coverage:
	mkdir -p coverage
	@echo "Running comprehensive coverage testing to achieve 80%+ coverage..."
	# Start server in background
	./server & SERVER_PID=$$!; \
	sleep 2; \
	echo "Testing various graph configurations and algorithm paths..."; \
	echo "=== Testing Random Graphs ==="; \
	./client -r -n 3 -e 3 || true; \
	./client -r -n 4 -e 5 || true; \
	./client -r -n 5 -e 7 || true; \
	./client -r -n 6 -e 8 || true; \
	./client -r -n 8 -e 10 || true; \
	./client -r -n 10 -e 15 || true; \
	./client -r -n 12 -e 20 || true; \
	echo "=== Testing Dense Graphs ==="; \
	./client -r -n 5 -e 10 || true; \
	./client -r -n 6 -e 15 || true; \
	echo "=== Testing Sparse Graphs ==="; \
	./client -r -n 8 -e 5 || true; \
	./client -r -n 10 -e 8 || true; \
	echo "=== Testing Edge Cases ==="; \
	./client -r -n 2 -e 1 || true; \
	./client -r -n 3 -e 0 || true; \
	./client -r -n 1 -e 0 || true; \
	echo "=== Testing Different Sizes ==="; \
	./client -r -n 7 -e 12 || true; \
	./client -r -n 9 -e 18 || true; \
	./client -r -n 11 -e 25 || true; \
	sleep 1; \
	kill $$SERVER_PID 2>/dev/null || true; \
	sleep 1
	@echo "Comprehensive coverage testing complete!"
	@echo "Running enhanced comprehensive algorithm tests for maximum coverage..."
	./test_enhanced_coverage
	@echo "Enhanced algorithm testing complete!"

# Comprehensive coverage target that tests ALL code paths
comprehensive-coverage: clean build-coverage run-coverage run-algorithm-tests run-comprehensive-coverage-tests generate-coverage-report

# Run comprehensive tests that hit all error paths and edge cases  
run-comprehensive-coverage-tests:
	@echo "=== RUNNING COMPREHENSIVE COVERAGE TESTS ==="
	@echo "Building comprehensive test programs..."
	$(MAKE) build-coverage-tests
	@echo "Running client coverage tests..."
	./test_client_coverage || true
	@echo "Running server coverage tests..."  
	./test_server_coverage || true
	@echo "Running comprehensive server/client tests..."
	./test_server_client_comprehensive || true
	@echo "=== COMPREHENSIVE COVERAGE TESTS COMPLETED ==="

# Generate coverage reports (only for project files)
generate-coverage-report:
	@echo "Generating coverage reports for ALL project files (.cpp and .hpp)..."
	# Generate coverage for all .cpp files individually
	@for file in *.cpp; do \
		if [ -f "$$file" ]; then \
			echo "Processing $$file..."; \
			gcov -o . "$$file" 2>/dev/null || true; \
		fi; \
	done
	# Also generate coverage for header files that might have inline implementations
	@for file in *.hpp; do \
		if [ -f "$$file" ]; then \
			echo "Processing $$file..."; \
			gcov -o . "$$file" 2>/dev/null || true; \
		fi; \
	done
	# Try to generate coverage from object files for headers
	@echo "Generating coverage from object files..."
	@for obj in *.o; do \
		if [ -f "$$obj" ]; then \
			gcov -o . "$$obj" 2>/dev/null || true; \
		fi; \
	done
	mkdir -p coverage
	@echo "Moving project .gcov files to coverage directory..."
	# Move all project .gcov files to coverage directory
	@for file in *.gcov; do \
		base_name=$$(echo "$$file" | sed 's/\.gcov$$//'); \
		if [ -f "$$base_name" ] || echo "$$file" | grep -q "^[A-Za-z].*\.cpp\.gcov$$\|^[A-Za-z].*\.hpp\.gcov$$"; then \
			echo "Moving $$file to coverage/"; \
			mv "$$file" coverage/ 2>/dev/null || true; \
		fi; \
	done
	@echo "Coverage generation complete for ALL project files!"
	@echo "Generated coverage files:"
	@ls -la coverage/*.gcov 2>/dev/null || echo "No .gcov files found"

# Generate detailed coverage summary with gcov file analysis
generate-detailed-summary:
	@echo "=== DETAILED COVERAGE SUMMARY ===" > coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "Generated on: $$(date)" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "PROJECT SOURCE FILES (.cpp) ANALYSIS:" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@for gcov_file in coverage/*.cpp.gcov; do \
		if [ -f "$$gcov_file" ]; then \
			filename=$$(basename "$$gcov_file" .gcov); \
			echo "File: $$filename" >> coverage/DETAILED_COVERAGE_SUMMARY.txt; \
			echo "----------------------------------------" >> coverage/DETAILED_COVERAGE_SUMMARY.txt; \
			# Extract coverage percentage and line counts from gcov file
			if grep -q "Lines executed:" "$$gcov_file"; then \
				grep "Lines executed:" "$$gcov_file" | head -1 >> coverage/DETAILED_COVERAGE_SUMMARY.txt; \
			else \
				echo "Lines executed: Coverage data not available" >> coverage/DETAILED_COVERAGE_SUMMARY.txt; \
			fi; \
			# Extract source file info
			source_line=$$(grep "Source:" "$$gcov_file" | head -1); \
			if [ -n "$$source_line" ]; then \
				echo "$$source_line" >> coverage/DETAILED_COVERAGE_SUMMARY.txt; \
			fi; \
			# Count total lines, executed lines, and missed lines
			total_lines=$$(grep -c "^[[:space:]]*[0-9#-]*:" "$$gcov_file" 2>/dev/null || echo "0"); \
			executed_lines=$$(grep -c "^[[:space:]]*[0-9][0-9]*:" "$$gcov_file" 2>/dev/null || echo "0"); \
			missed_lines=$$(grep -c "^[[:space:]]*#####:" "$$gcov_file" 2>/dev/null || echo "0"); \
			echo "Total source lines: $$total_lines" >> coverage/DETAILED_COVERAGE_SUMMARY.txt; \
			echo "Executed lines: $$executed_lines" >> coverage/DETAILED_COVERAGE_SUMMARY.txt; \
			echo "Missed lines: $$missed_lines" >> coverage/DETAILED_COVERAGE_SUMMARY.txt; \
			echo "" >> coverage/DETAILED_COVERAGE_SUMMARY.txt; \
		fi; \
	done
	@echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "PROJECT HEADER FILES (.hpp) ANALYSIS:" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@for gcov_file in coverage/*.hpp.gcov; do \
		if [ -f "$$gcov_file" ]; then \
			filename=$$(basename "$$gcov_file" .gcov); \
			echo "File: $$filename" >> coverage/DETAILED_COVERAGE_SUMMARY.txt; \
			echo "----------------------------------------" >> coverage/DETAILED_COVERAGE_SUMMARY.txt; \
			# Extract coverage info
			if grep -q "Lines executed:" "$$gcov_file"; then \
				grep "Lines executed:" "$$gcov_file" | head -1 >> coverage/DETAILED_COVERAGE_SUMMARY.txt; \
			else \
				echo "Lines executed: No executable lines or coverage data not available" >> coverage/DETAILED_COVERAGE_SUMMARY.txt; \
			fi; \
			# Extract source file info
			source_line=$$(grep "Source:" "$$gcov_file" | head -1); \
			if [ -n "$$source_line" ]; then \
				echo "$$source_line" >> coverage/DETAILED_COVERAGE_SUMMARY.txt; \
			fi; \
			echo "" >> coverage/DETAILED_COVERAGE_SUMMARY.txt; \
		fi; \
	done
	@echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "FILES ACHIEVING 80%+ COVERAGE:" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@for gcov_file in coverage/*.gcov; do \
		if [ -f "$$gcov_file" ]; then \
			filename=$$(basename "$$gcov_file" .gcov); \
			coverage_line=$$(grep "Lines executed:" "$$gcov_file" | head -1 | grep -o "[0-9]*\.[0-9]*%" | head -1); \
			if [ -n "$$coverage_line" ]; then \
				coverage_num=$$(echo "$$coverage_line" | sed 's/%//'); \
				if [ $$(echo "$$coverage_num >= 80" | bc 2>/dev/null || echo "0") -eq 1 ]; then \
					echo "✓ $$filename: $$coverage_line" >> coverage/DETAILED_COVERAGE_SUMMARY.txt; \
				fi; \
			fi; \
		fi; \
	done
	@echo "" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "FILES BELOW 80% COVERAGE:" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@for gcov_file in coverage/*.gcov; do \
		if [ -f "$$gcov_file" ]; then \
			filename=$$(basename "$$gcov_file" .gcov); \
			coverage_line=$$(grep "Lines executed:" "$$gcov_file" | head -1 | grep -o "[0-9]*\.[0-9]*%" | head -1); \
			if [ -n "$$coverage_line" ]; then \
				coverage_num=$$(echo "$$coverage_line" | sed 's/%//'); \
				if [ $$(echo "$$coverage_num < 80" | bc 2>/dev/null || echo "1") -eq 1 ]; then \
					echo "✗ $$filename: $$coverage_line" >> coverage/DETAILED_COVERAGE_SUMMARY.txt; \
				fi; \
			else \
				echo "? $$filename: Coverage data not available" >> coverage/DETAILED_COVERAGE_SUMMARY.txt; \
			fi; \
		fi; \
	done
	@echo "" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "END OF DETAILED ANALYSIS" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
	@echo "Detailed coverage summary generated in coverage/DETAILED_COVERAGE_SUMMARY.txt"

# Generate HTML coverage report (filtered for project files only)
coverage-html: coverage
	@echo "Generating HTML coverage report for project files only..."
	mkdir -p coverage/html
	# Capture coverage data
	lcov --capture --directory . --output-file coverage/coverage_raw.info
	# Extract only our project files (current directory)
	lcov --extract coverage/coverage_raw.info "$(shell pwd)/*.cpp" "$(shell pwd)/*.hpp" --output-file coverage/coverage_project.info
	# Remove any system includes that might have slipped through
	lcov --remove coverage/coverage_project.info '/usr/*' '/opt/*' '**/bits/*' '**/c++/*' --output-file coverage/coverage_filtered.info
	# Generate HTML report
	genhtml coverage/coverage_filtered.info --output-directory coverage/html \
		--title "Project Code Coverage Report" \
		--show-details --legend --demangle-cpp \
		--prefix $(shell pwd)
	@echo "====================================="
	@echo "Coverage report generated successfully!"
	@echo "Open coverage/html/index.html in a browser"
	@echo "====================================="

# Show coverage summary for project files only
coverage-summary: coverage
	@echo "========================================="
	@echo "CODE COVERAGE SUMMARY (Project Files Only)"
	@echo "========================================="
	@for file in coverage/*.cpp.gcov coverage/*.hpp.gcov; do \
		if [ -f "$$file" ]; then \
			basename_file=$$(basename "$$file" .gcov); \
			echo ""; \
			echo "File: $$basename_file"; \
			echo "----------------------------------------"; \
			total_lines=$$(grep -E "^ *[0-9]+:" "$$file" | wc -l); \
			uncovered_lines=$$(grep -E "^ *#####:" "$$file" | wc -l); \
			covered_lines=$$((total_lines - uncovered_lines)); \
			echo "  Total executable lines: $$total_lines"; \
			echo "  Covered lines: $$covered_lines"; \
			echo "  Uncovered lines: $$uncovered_lines"; \
			if [ $$total_lines -gt 0 ]; then \
				coverage_percent=$$((covered_lines * 100 / total_lines)); \
				echo "  Coverage: $$coverage_percent%"; \
			else \
				echo "  Coverage: N/A (no executable lines)"; \
			fi; \
		fi; \
	done
	@echo ""
	@echo "========================================="

# Clean coverage files
clean-coverage:
	rm -f *.gcda *.gcno *.gcov
	rm -rf coverage/
	rm -f *.o server client test_coverage test_enhanced_coverage

# Enhanced clean target
clean: clean-coverage
	rm -f *.o server client test_coverage

.PHONY: all clean valgrind-memcheck-server valgrind-helgrind-server valgrind-cg-server valgrind-memcheck-client valgrind-helgrind-client valgrind-cg-client valgrind-all coverage build-coverage server-coverage client-coverage test-coverage run-coverage run-algorithm-tests generate-coverage-report coverage-html coverage-summary clean-coverage comprehensive-coverage
