# Concurrent Systems & Graph Algorithms

Co-built by **Dor Cohen and Baruh Ifraimov**. The repository presents our shared systems-programming work; the contribution history records individual changes.

> A C++ operating-systems project that evolves from graph algorithms into networked, concurrent server architectures and then validates them with profiling, race-detection, and coverage tooling.

![C++](https://img.shields.io/badge/C%2B%2B-Systems_Programming-00599C?style=flat-square&logo=cplusplus&logoColor=white)
![Concurrency](https://img.shields.io/badge/concurrency-threads_%C2%B7_queues_%C2%B7_condition_variables-555?style=flat-square)
![Tooling](https://img.shields.io/badge/tooling-Valgrind_%C2%B7_Helgrind_%C2%B7_gcov_%C2%B7_gprof-7A1?style=flat-square)

## Why this project matters

This repository is more than a set of isolated operating-systems exercises. The later stages build a **TCP client/server system for graph processing**, progressively introduce design patterns and concurrency models, and validate behavior with real systems tooling.

The graph layer supports multiple algorithms — including **minimum spanning tree, max flow, strongly connected components, and clique counting** — while the server implementations explore different ways to schedule and coordinate work.

## Architecture progression

| Stage | Engineering focus |
| --- | --- |
| Graph core | Graph representation and algorithmic processing |
| Client / server | TCP-based request flow between clients and a graph-processing server |
| Strategy / Factory | Runtime selection of graph algorithms behind a common interface |
| Leader–Follower | Worker threads coordinate through a shared leader token / condition variable |
| Active Object pipeline | Work flows through four concurrent stages: **MST → MaxFlow → SCC → Clique** |
| Validation | Memory, race, profiling, and coverage analysis with **Valgrind / Helgrind / Callgrind / gcov / gprof** |

## Engineering highlights

- C++11 systems programming with sockets and threads
- TCP client/server communication
- Synchronization using mutexes and condition variables
- Blocking queues and multi-stage processing
- Strategy / Factory-style algorithm abstraction
- Leader–Follower concurrency model
- Active Object pipeline architecture
- Graph algorithms implemented behind reusable interfaces
- Memory / thread analysis and profiling artifacts committed for inspection

## Repository map

- `Question6/` — client/server baseline
- `Question7/` — graph-algorithm abstraction and factory
- `Question8/` — Leader–Follower server
- `Question9/` — Active Object pipeline
- `Question10/` — concurrent pipeline with Valgrind / Helgrind / Callgrind result artifacts
- `Question11/` — coverage-enabled build and gcov results

## Build

Build the complete project from the repository root:

```bash
make
```

Or build an individual stage:

```bash
make -C Question9
```

## What to inspect first

If you're reviewing this repository as part of a software-engineering portfolio:

1. `Question8/server.cpp` — Leader–Follower synchronization.
2. `Question9/server.cpp` — the four-stage Active Object pipeline.
3. `Question11/GraphAlgorithmFactory.*` — graph-algorithm abstraction.
4. `Question10/results/` and `Question11/results_gcov/` — validation / profiling evidence.
