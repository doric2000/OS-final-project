/*
** test_server_coverage.cpp -- Test server.cpp with proper signal handling
** This will start the server, exercise its functions, and terminate it properly
*/

#include <iostream>
#include <thread>
#include <chrono>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <vector>

void send_test_graph_to_server(int port = 3490) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Let server start
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) return;
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0) {
        // Send a test graph to exercise the server pipeline
        std::string test_graph = 
            "12345\n"      // seed
            "1\n"          // directed
            "4\n"          // vertices  
            "5\n"          // edges
            "0 1 10\n"     // edge data
            "1 2 20\n"
            "2 3 30\n"
            "3 0 40\n"
            "1 3 15\n";
        
        send(sockfd, test_graph.c_str(), test_graph.size(), 0);
        
        // Read response
        char buffer[4096];
        recv(sockfd, buffer, sizeof(buffer), 0);
        
        std::cout << "Successfully sent graph to server and received response" << std::endl;
    }
    
    close(sockfd);
}

int main() {
    std::cout << "=== SERVER COVERAGE TEST ===" << std::endl;
    
    // Build server first
    system("make server 2>/dev/null");
    
    // Start server in background
    pid_t server_pid = fork();
    if (server_pid == 0) {
        // Child process - run server
        execl("./server", "./server", (char*)nullptr);
        exit(1);
    } else if (server_pid > 0) {
        std::cout << "Started server with PID: " << server_pid << std::endl;
        
        // Give server time to start
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        
        // Send multiple test graphs to exercise different paths
        std::cout << "Sending test graphs..." << std::endl;
        
        // Test 1: Basic directed graph
        send_test_graph_to_server();
        
        // Test 2: Different graph
        std::thread([]{
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            int sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd != -1) {
                struct sockaddr_in server_addr;
                memset(&server_addr, 0, sizeof(server_addr));
                server_addr.sin_family = AF_INET;
                server_addr.sin_port = htons(3490);
                server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0) {
                    std::string test_graph = 
                        "54321\n"
                        "0\n"
                        "3\n"
                        "3\n"
                        "0 1 5\n"
                        "1 2 10\n"
                        "2 0 15\n";
                    
                    send(sockfd, test_graph.c_str(), test_graph.size(), 0);
                    
                    char buffer[4096];
                    recv(sockfd, buffer, sizeof(buffer), 0);
                    
                    std::cout << "Sent second graph to server" << std::endl;
                }
                close(sockfd);
            }
        }).join();
        
        // Test 3: Stress test with multiple concurrent connections
        std::vector<std::thread> threads;
        for (int i = 0; i < 3; i++) {
            threads.emplace_back([i]{
                std::this_thread::sleep_for(std::chrono::milliseconds(200 + i * 50));
                int sockfd = socket(AF_INET, SOCK_STREAM, 0);
                if (sockfd != -1) {
                    struct sockaddr_in server_addr;
                    memset(&server_addr, 0, sizeof(server_addr));
                    server_addr.sin_family = AF_INET;
                    server_addr.sin_port = htons(3490);
                    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                    
                    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0) {
                        std::string test_graph = 
                            std::to_string(1000 + i) + "\n"
                            "1\n"
                            "5\n"
                            "6\n"
                            "0 1 1\n"
                            "1 2 2\n"
                            "2 3 3\n"
                            "3 4 4\n"
                            "4 0 5\n"
                            "1 4 6\n";
                        
                        send(sockfd, test_graph.c_str(), test_graph.size(), 0);
                        
                        char buffer[4096];
                        recv(sockfd, buffer, sizeof(buffer), 0);
                        
                        std::cout << "Completed concurrent test " << i << std::endl;
                    }
                    close(sockfd);
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        std::cout << "All tests completed. Shutting down server..." << std::endl;
        
        // Send termination signal to server
        kill(server_pid, SIGUSR1);
        
        // Wait a bit for graceful shutdown
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // If still running, force kill
        int status;
        if (waitpid(server_pid, &status, WNOHANG) == 0) {
            std::cout << "Server didn't exit gracefully, sending SIGTERM..." << std::endl;
            kill(server_pid, SIGTERM);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            
            if (waitpid(server_pid, &status, WNOHANG) == 0) {
                std::cout << "Force killing server..." << std::endl;
                kill(server_pid, SIGKILL);
            }
        }
        
        waitpid(server_pid, &status, 0);
        std::cout << "Server terminated." << std::endl;
    }
    
    std::cout << "=== SERVER COVERAGE TEST COMPLETED ===" << std::endl;
    return 0;
}
