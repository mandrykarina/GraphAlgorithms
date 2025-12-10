#include <iostream>
#include <string>

// Включи заголовки графов
#include "graph/graph.h"
#include "graph/vertex.h"
#include "graph/edge.h"
#include "algorithms/shortest_path.h"
#include "algorithms/tsp.h"
#include "algorithms/spanning_tree.h"
#include "algorithms/connectivity.h"
#include "algorithms/coloring.h"
#include "algorithms/hotel_optimization.h"
#include "utils/timer.h"
#include "tests/benchmark.h"

using Graph = UndirectedGraph<double>;
using ShortestPath = ShortestPathFinder<double>;
using TSP = TSPSolver<double>;
using MST = SpanningTreeFinder<double>;
using Connectivity = ConnectivityFinder<double>;
using Coloring = GraphColorer<double>;
using Hotel = HotelOptimization<double>;

int main()
{
    std::cout << "\n╔══════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                  BENCHMARK TESTS - STANDALONE                       ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════╝" << std::endl;

    // Запускаем бенчмарки
    Benchmark::runAllBenchmarks();

    return 0;
}
