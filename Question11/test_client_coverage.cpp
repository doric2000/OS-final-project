/*
** test_client_coverage.cpp -- Test client.cpp with all argument combinations
** This will exercise all code paths in client.cpp including error conditions
*/

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <string>

void test_client_with_args(const std::vector<std::string>& args) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process - exec client with arguments
        std::vector<char*> argv;
        argv.push_back(const_cast<char*>("./client"));
        
        for (const auto& arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);
        
        // Redirect stderr to /dev/null to avoid spam
        freopen("/dev/null", "w", stderr);
        
        execv("./client", argv.data());
        exit(1); // If exec fails
    } else if (pid > 0) {
        // Parent process - wait for child
        int status;
        waitpid(pid, &status, 0);
    }
}

int main() {
    std::cout << "=== CLIENT COVERAGE TEST ===" << std::endl;
    std::cout << "Testing all client argument combinations..." << std::endl;
    
    // Build client first
    system("make client 2>/dev/null");
    
    // Test cases that hit different code paths
    std::vector<std::vector<std::string>> test_cases = {
        // Test IPv6 hostname (hits IPv6 path in get_in_addr)
        {"-h", "::1"},
        
        // Test manual mode flag
        {"-m"},
        
        // Test weight argument
        {"-w", "100"},
        
        // Test seed argument
        {"-s", "12345"},
        
        // Test invalid argument (hits default case)
        {"-x"},
        
        // Test manual mode with missing edges (error path)
        {"-m", "-v", "5"},
        
        // Test manual mode with zero vertices (error path)
        {"-m", "-v", "0", "-e", "5"},
        
        // Test manual mode with negative vertices (error path)
        {"-m", "-v", "-1", "-e", "5"},
        
        // Test manual mode with zero edges (error path)
        {"-m", "-v", "5", "-e", "0"},
        
        // Test manual mode with negative edges (error path)
        {"-m", "-v", "5", "-e", "-1"},
        
        // Test directed graph with too many edges
        {"-d", "-v", "3", "-e", "10"},
        
        // Test undirected graph with too many edges
        {"-v", "3", "-e", "10"},
        
        // Test valid manual mode directed
        {"-m", "-d", "-v", "3", "-e", "3"},
        
        // Test valid manual mode undirected
        {"-m", "-v", "3", "-e", "2"},
        
        // Test random mode with all options
        {"-d", "-v", "5", "-e", "8", "-w", "50", "-s", "999"},
        
        // Test connection to non-existent server (hits connection error paths)
        {"-h", "192.0.2.1"}, // RFC5737 test address that won't exist
        
        // Test invalid hostname (hits getaddrinfo error)
        {"-h", "invalid.hostname.that.does.not.exist.999"},
    };
    
    for (size_t i = 0; i < test_cases.size(); i++) {
        std::cout << "Test case " << (i+1) << ": ";
        for (const auto& arg : test_cases[i]) {
            std::cout << arg << " ";
        }
        std::cout << std::endl;
        
        test_client_with_args(test_cases[i]);
    }
    
    std::cout << "=== CLIENT COVERAGE TEST COMPLETED ===" << std::endl;
    return 0;
}
