/*
** server.cpp -- pipeline server: 4 Active Objects (MST -> MaxFlow -> SCC -> Clique)
** Sync back to client thread via condition_variable (no std::promise/future).
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/select.h>

#include <vector>
#include <sstream>
#include <set>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>
#include <atomic>

#include "Graph.hpp"
#include "MSTAlgorithm.hpp"
#include "MaxFlowAlgorithm.hpp"
#include "SCCAlgorithm.hpp"
#include "CliqueCountAlgorithm.hpp"

#define PORT "3490"
#define BACKLOG 10

// Global flag to signal shutdown
std::atomic<bool> should_exit(false);

// Function to check for keyboard input without blocking
bool kbhit() {
    int ch = getchar();
    if (ch != EOF) {
        ungetc(ch, stdin);
        return true;
    }
    return false;
}

// Function to set stdin to non-blocking mode
void set_stdin_nonblocking() {
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

// Function to restore stdin to blocking mode
void set_stdin_blocking() {
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
}

static void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET)  return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Read full message: 4 headers (seed, directed, vertices, edges) + <edges> lines: "u v w"
static bool recv_full_message(int fd, std::string& out) {
    out.clear();
    char buf[4096];
    int expected_lines = -1;

    for (;;) {
        int n = recv(fd, buf, sizeof(buf)-1, 0);
        if (n <= 0) break;
        buf[n] = '\0';
        out += buf;

        int lc = 0; for (char c : out) if (c=='\n') ++lc;
        if (expected_lines == -1 && lc >= 4) {
            std::istringstream peek(out);
            std::string tmp;
            std::getline(peek,tmp); // seed
            std::getline(peek,tmp); // directed
            std::getline(peek,tmp); // vertices
            std::getline(peek,tmp); // edges
            int edges = atoi(tmp.c_str());
            expected_lines = 4 + edges;
        }
        if (expected_lines != -1) {
            int lc2 = 0; for (char c : out) if (c=='\n') ++lc2;
            if (lc2 >= expected_lines) break;
        }
    }

    if (expected_lines == -1) return false;
    int lc = 0; for (char c : out) if (c=='\n') ++lc;
    return lc >= expected_lines;
}

template <typename T>
class BlockingQueue {
public:
    void push(T x) {
        std::unique_lock<std::mutex> lk(m_);
        q_.push(std::move(x));
        cv_.notify_one();
    }
    T pop() {
        std::unique_lock<std::mutex> lk(m_);
        cv_.wait(lk, [&]{ return !q_.empty() || should_exit.load(); });
        if (should_exit.load() && q_.empty()) {
            return T{}; // return default-constructed object for graceful shutdown
        }
        T x = std::move(q_.front());
        q_.pop();
        return x;
    }
    void shutdown() {
        std::unique_lock<std::mutex> lk(m_);
        cv_.notify_all();
    }
private:
    std::mutex m_;
    std::condition_variable cv_;
    std::queue<T> q_;
};

// Shared between the client-handling thread and the pipeline stages.
// We use cv+mutex to notify the client thread when the last stage is done.
struct Job {
    std::shared_ptr<Graph::Graph> graph;

    // per-algorithm results (filled by stages in order)
    std::string mst, maxflow, scc, clique;

    // ---- cv-based reply signaling ----
    std::mutex mtx;                 // protects 'done' and 'reply'
    std::condition_variable cv;     // client thread waits on this
    bool done = false;              // set true by the last stage
    std::string reply;              // final aggregated reply
};
using JobPtr = std::shared_ptr<Job>; // alias for std::shared_ptr<Job>

BlockingQueue<JobPtr> Q_mst;
BlockingQueue<JobPtr> Q_maxflow;
BlockingQueue<JobPtr> Q_scc;
BlockingQueue<JobPtr> Q_clique;

// Each stage pops a Job, runs its algorithm, stores the result, and pushes to the next queue.

static void mst_stage() {
    MSTAlgorithm alg;
    while (!should_exit.load()) {
        JobPtr job = Q_mst.pop();
        if (should_exit.load() || !job) break;
        job->mst = alg.run(*job->graph);
        Q_maxflow.push(job);
    }
}

static void maxflow_stage() {
    MaxFlowAlgorithm alg;
    while (!should_exit.load()) {
        JobPtr job = Q_maxflow.pop();
        if (should_exit.load() || !job) break;
        job->maxflow = alg.run(*job->graph);
        Q_scc.push(job);
    }
}

static void scc_stage() {
    SCCAlgorithm alg;
    while (!should_exit.load()) {
        JobPtr job = Q_scc.pop();
        if (should_exit.load() || !job) break;
        job->scc = alg.run(*job->graph);
        Q_clique.push(job);
    }
}

static void clique_stage() {
    CliqueCountAlgorithm alg;
    while (!should_exit.load()) {
        JobPtr job = Q_clique.pop();
        if (should_exit.load() || !job) break;
        job->clique = alg.run(*job->graph);

        // Build final reply and notify the waiting client thread.
        std::ostringstream out;
        out << "mst: "      << job->mst
            << "\nmaxflow: " << job->maxflow
            << "\nscc: "     << job->scc
            << "\nclique: "  << job->clique
            << "\n";

            // ensures that the 'reply' and 'done' will be executed together (fully executed)
        {
            std::lock_guard<std::mutex> lk(job->mtx);
            job->reply = out.str();
            job->done  = true;
        }
        job->cv.notify_one(); // notify AFTER releasing the lock
    }
}

// Reads request, builds Job, enqueues, waits on cv until the last stage fills 'reply'.
static void handle_client(int new_fd) {
    std::string full_data;
    if (!recv_full_message(new_fd, full_data)) {
        const char* err = "Error: incomplete or bad message\n";
        send(new_fd, err, strlen(err), 0);
        close(new_fd);
        return;
    }

    // Parse headers
    std::istringstream iss(full_data);
    std::string line;

    std::getline(iss, line); /* seed (unused) */
    std::getline(iss, line); 
    int directed  = atoi(line.c_str());
    std::getline(iss, line); 
    int num_vertices = atoi(line.c_str());
    std::getline(iss, line); 
    int num_edges = atoi(line.c_str());

    // Build graph
    auto job = std::make_shared<Job>();
    job->graph = std::make_shared<Graph::Graph>(num_vertices, directed);

    // (generating edges) check used edges, if edge is already exists ignore, else create
    std::set<std::pair<int,int>> used;
    for (int i = 0; i < num_edges; ++i) {
        if (!std::getline(iss, line)) break;
        std::istringstream lss(line);
        int u, v, w; lss >> u >> v >> w;
        if (!lss || u<0 || u>=num_vertices || v<0 || v>=num_vertices || w<=0) continue;
        if (u==v || used.count({u,v})) continue;
        job->graph->addEdge(u, v, w);
        used.insert({u,v});
    }

    // Print the received graph
    job->graph->printGraph();

    // Enqueue into stage #1 (first blocking queue, pushing the first job)
    Q_mst.push(job);

    // Wait until the last stage marks done=true and sets reply.
    std::string reply;
    {
        std::unique_lock<std::mutex> lk(job->mtx);
        job->cv.wait(lk, [&]{ return job->done; }); // handles spurious wakeups (kistuah)
        reply = job->reply; // copy out while locked
    }

    // Send and close
    send(new_fd, reply.c_str(), reply.size(), 0);
    close(new_fd);
}

int main(void) {
    // Create listening socket (same as before)
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int yes = 1, rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket"); continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt"); exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd); perror("server: bind"); continue;
        }
        break;
    }

    freeaddrinfo(servinfo);
    if (p == NULL) { fprintf(stderr, "server: failed to bind\n"); exit(1); }
    if (listen(sockfd, BACKLOG) == -1) { perror("listen"); exit(1); }

    printf("server: waiting for connections...\n");
    printf("Press 'x' + Enter to exit gracefully\n");

    // Set stdin to non-blocking mode
    set_stdin_nonblocking();

    // Start exactly 4 Active Objects == pipeline stages
    std::thread t_mst(mst_stage);
    std::thread t_mf(maxflow_stage);
    std::thread t_scc(scc_stage);
    std::thread t_cq(clique_stage);

    // Accept loop: spawn a short-lived thread per client (keeps accept responsive)
    while (!should_exit.load()) {
        // Check for keyboard input
        if (kbhit()) {
            char ch = getchar();
            if (ch == 'x' || ch == 'X') {
                printf("\nExiting server gracefully...\n");
                should_exit.store(true);
                break;
            }
        }

        // Set socket to non-blocking for accept
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; // 100ms timeout
        
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        
        int result = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
        if (result > 0 && FD_ISSET(sockfd, &readfds)) {
            struct sockaddr_storage their_addr;
            socklen_t sin_size = sizeof their_addr;
            int new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
            if (new_fd == -1) { 
                perror("accept"); 
                continue; 
            }

            char s[INET6_ADDRSTRLEN];
            inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
            printf("server: got connection from %s\n", s);

            std::thread(handle_client, new_fd).detach();
        }
    }

    // Restore stdin to blocking mode
    set_stdin_blocking();

    // Shutdown the queues to wake up blocked threads
    Q_mst.shutdown();
    Q_maxflow.shutdown();
    Q_scc.shutdown();
    Q_clique.shutdown();

    // Close the listening socket
    close(sockfd);

    // Wait for all worker threads to finish
    printf("Waiting for worker threads to finish...\n");
    t_mst.join(); 
    t_mf.join(); 
    t_scc.join(); 
    t_cq.join();

    printf("Server shut down complete.\n");
    return 0;
}
