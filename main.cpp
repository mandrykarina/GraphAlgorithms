#include <iostream>
#include <iomanip>

// Включаем все заголовки
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
#include "tests/test_graph.cpp"
#include "tests/test_algorithms.cpp"

using Graph = UndirectedGraph<double>;
using ShortestPath = ShortestPathFinder<double>;
using TSP = TSPSolver<double>;
using MST = SpanningTreeFinder<double>;
using Connectivity = ConnectivityFinder<double>;
using Coloring = GraphColorer<double>;
using Hotel = HotelOptimization<double>;

// ============================================================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ СОЗДАНИЯ ТЕСТОВЫХ ГРАФОВ
// ============================================================================

/**
 * Создать простой граф для первичных тестов
 */
Graph createSimpleTestGraph()
{
    Graph g;

    // Добавляем вершины
    for (int i = 0; i < 5; ++i)
    {
        g.addVertex(i, "V" + std::to_string(i));
    }

    // Добавляем ребра (взвешенные)
    g.addEdge(0, 1, 2.0);
    g.addEdge(0, 2, 4.0);
    g.addEdge(1, 2, 1.0);
    g.addEdge(1, 3, 7.0);
    g.addEdge(2, 3, 2.0);
    g.addEdge(3, 4, 1.0);

    return g;
}

/**
 * Создать граф для TSP
 */
Graph createTSPGraph()
{
    Graph g;

    // Полносвязный граф - идеален для TSP
    for (int i = 0; i < 6; ++i)
    {
        g.addVertex(i, "City" + std::to_string(i));
    }

    // Добавляем ребра между всеми вершинами
    double distances[6][6] = {
        {0, 10, 15, 20, 25, 30},
        {10, 0, 35, 25, 15, 40},
        {15, 35, 0, 30, 40, 20},
        {20, 25, 30, 0, 15, 25},
        {25, 15, 40, 15, 0, 10},
        {30, 40, 20, 25, 10, 0}};

    for (int i = 0; i < 6; ++i)
    {
        for (int j = i + 1; j < 6; ++j)
        {
            g.addEdge(i, j, distances[i][j]);
        }
    }

    return g;
}

/**
 * Создать несвязный граф для тестирования компонент
 */
Graph createDisconnectedGraph()
{
    Graph g;

    // Компонента 1: 0-1-2
    for (int i = 0; i < 3; ++i)
    {
        g.addVertex(i, "C1_V" + std::to_string(i));
    }
    g.addEdge(0, 1, 1.0);
    g.addEdge(1, 2, 1.0);

    // Компонента 2: 3-4
    for (int i = 3; i < 5; ++i)
    {
        g.addVertex(i, "C2_V" + std::to_string(i));
    }
    g.addEdge(3, 4, 2.0);

    // Компонента 3: 5 (изолированная)
    g.addVertex(5, "C3_V5");

    return g;
}

/**
 * Создать граф для раскраски
 */
Graph createColoringGraph()
{
    Graph g;

    for (int i = 0; i < 7; ++i)
    {
        g.addVertex(i, "V" + std::to_string(i));
    }

    // Создаем граф, который требует минимум 3 цвета
    g.addEdge(0, 1, 1.0);
    g.addEdge(0, 2, 1.0);
    g.addEdge(1, 2, 1.0);
    g.addEdge(1, 3, 1.0);
    g.addEdge(2, 3, 1.0);
    g.addEdge(3, 4, 1.0);
    g.addEdge(4, 5, 1.0);
    g.addEdge(5, 6, 1.0);
    g.addEdge(6, 4, 1.0);

    return g;
}

// ============================================================================
// ОСНОВНЫЕ ТЕСТОВЫЕ ФУНКЦИИ
// ============================================================================

void testGraphBasics()
{
    std::cout << "\n"
              << std::string(70, '=') << std::endl;
    std::cout << "TEST 1: GRAPH BASICS" << std::endl;
    std::cout << std::string(70, '=') << std::endl;

    Graph g = createSimpleTestGraph();

    std::cout << "\nGraph Info:" << std::endl;
    g.printInfo();

    std::cout << "\nBasic queries:" << std::endl;
    std::cout << "Vertex 0 label: " << g.getVertex(0).label << std::endl;
    std::cout << "Edge 0-1 weight: " << g.getEdgeWeight(0, 1) << std::endl;
    std::cout << "Neighbors of 1: ";
    for (int v : g.getAdjacentVertices(1))
    {
        std::cout << v << " ";
    }
    std::cout << std::endl;
}

void testShortestPath()
{
    std::cout << "\n"
              << std::string(70, '=') << std::endl;
    std::cout << "TEST 2: SHORTEST PATH (Dijkstra & BFS)" << std::endl;
    std::cout << std::string(70, '=') << std::endl;

    Graph g = createSimpleTestGraph();

    // Тест Dijkstra
    std::cout << "\n--- Dijkstra Algorithm ---" << std::endl;
    Timer timer;
    timer.start();

    auto pathResult = ShortestPath::findPath(g, 0, 4);

    timer.stop();
    std::cout << "Time: " << timer.elapsedMs() << " ms" << std::endl;
    ShortestPath::printPath(pathResult);

    // Все пути из вершины 0
    std::cout << "\nAll shortest paths from vertex 0:" << std::endl;
    auto allPaths = ShortestPath::dijkstra(g, 0);
    for (int v : g.getAllVertices())
    {
        std::cout << "  0 -> " << v << ": distance = " << allPaths.distances[v] << std::endl;
    }
}

void testTSP()
{
    std::cout << "\n"
              << std::string(70, '=') << std::endl;
    std::cout << "TEST 3: TRAVELING SALESMAN PROBLEM" << std::endl;
    std::cout << std::string(70, '=') << std::endl;

    Graph g = createTSPGraph();

    // Brute Force (для малого графа)
    std::cout << "\n--- Brute Force (Exact Solution) ---" << std::endl;
    Timer timer;
    timer.start();
    auto bruteForceSolution = TSP::bruteForce(g, 0);
    timer.stop();

    TSP::printResult(bruteForceSolution);
    std::cout << "Execution time: " << timer.elapsedMs() << " ms" << std::endl;

    // Nearest Neighbor
    std::cout << "\n--- Nearest Neighbor (Heuristic) ---" << std::endl;
    timer.reset();
    timer.start();
    auto nnSolution = TSP::nearestNeighbor(g, 0);
    timer.stop();

    TSP::printResult(nnSolution);
    std::cout << "Execution time: " << timer.elapsedMs() << " ms" << std::endl;

    // Hybrid (NN + 2-opt)
    std::cout << "\n--- Hybrid Solution (NN + 2-opt) ---" << std::endl;
    timer.reset();
    timer.start();
    auto hybridSolution = TSP::hybridSolver(g, 0);
    timer.stop();

    TSP::printResult(hybridSolution);
    std::cout << "Execution time: " << timer.elapsedMs() << " ms" << std::endl;
}

void testSpanningTree()
{
    std::cout << "\n"
              << std::string(70, '=') << std::endl;
    std::cout << "TEST 4: MINIMUM SPANNING TREE (Kruskal & Prim)" << std::endl;
    std::cout << std::string(70, '=') << std::endl;

    Graph g = createSimpleTestGraph();

    // Kruskal
    std::cout << "\n--- Kruskal Algorithm ---" << std::endl;
    Timer timer;
    timer.start();
    auto mstKruskal = MST::kruskal(g);
    timer.stop();

    MST::printMST(mstKruskal);
    std::cout << "Execution time: " << timer.elapsedMs() << " ms" << std::endl;

    // Prim
    std::cout << "\n--- Prim Algorithm ---" << std::endl;
    timer.reset();
    timer.start();
    auto mstPrim = MST::prim(g, 0);
    timer.stop();

    MST::printMST(mstPrim);
    std::cout << "Execution time: " << timer.elapsedMs() << " ms" << std::endl;
}

void testConnectivity()
{
    std::cout << "\n"
              << std::string(70, '=') << std::endl;
    std::cout << "TEST 5: CONNECTIVITY (Components)" << std::endl;
    std::cout << std::string(70, '=') << std::endl;

    // Связный граф
    std::cout << "\n--- Connected Graph ---" << std::endl;
    Graph g1 = createSimpleTestGraph();

    Timer timer;
    timer.start();
    auto conn1 = Connectivity::dfsFindComponents(g1);
    timer.stop();

    Connectivity::printConnectivity(conn1);
    std::cout << "Is connected: " << (Connectivity::isConnected(g1) ? "Yes" : "No") << std::endl;
    std::cout << "Execution time: " << timer.elapsedMs() << " ms" << std::endl;

    // Несвязный граф
    std::cout << "\n--- Disconnected Graph ---" << std::endl;
    Graph g2 = createDisconnectedGraph();

    timer.reset();
    timer.start();
    auto conn2 = Connectivity::dfsFindComponents(g2);
    timer.stop();

    Connectivity::printConnectivity(conn2);
    std::cout << "Is connected: " << (Connectivity::isConnected(g2) ? "Yes" : "No") << std::endl;
    std::cout << "Largest component size: " << Connectivity::getLargestComponentSize(g2) << std::endl;
    std::cout << "Execution time: " << timer.elapsedMs() << " ms" << std::endl;
}

void testColoring()
{
    std::cout << "\n"
              << std::string(70, '=') << std::endl;
    std::cout << "TEST 6: GRAPH COLORING" << std::endl;
    std::cout << std::string(70, '=') << std::endl;

    Graph g = createColoringGraph();

    // Greedy
    std::cout << "\n--- Greedy Coloring ---" << std::endl;
    Timer timer;
    timer.start();
    auto coloringGreedy = Coloring::greedyColoring(g);
    timer.stop();

    Coloring::printColoring(g, coloringGreedy);
    std::cout << "Execution time: " << timer.elapsedMs() << " ms" << std::endl;

    // Welsh-Powell
    std::cout << "\n--- Welsh-Powell Coloring ---" << std::endl;
    timer.reset();
    timer.start();
    auto coloringWP = Coloring::welshPowellColoring(g);
    timer.stop();

    Coloring::printColoring(g, coloringWP);
    std::cout << "Execution time: " << timer.elapsedMs() << " ms" << std::endl;
}

void testHotelOptimization()
{
    std::cout << "\n"
              << std::string(70, '=') << std::endl;
    std::cout << "TEST 7: HOTEL OPTIMIZATION (Dominating Set & K-Centers)" << std::endl;
    std::cout << std::string(70, '=') << std::endl;

    Graph g = createSimpleTestGraph();

    // Dominating Set
    std::cout << "\n--- Dominating Set Greedy ---" << std::endl;
    Timer timer;
    timer.start();
    auto assignment1 = Hotel::dominatingSetGreedy(g);
    timer.stop();

    Hotel::printAssignment(assignment1);
    std::cout << "Execution time: " << timer.elapsedMs() << " ms" << std::endl;

    // K-Centers (k=2)
    std::cout << "\n--- K-Centers (k=2) ---" << std::endl;
    timer.reset();
    timer.start();
    auto assignment2 = Hotel::kCenters(g, 2);
    timer.stop();

    Hotel::printAssignment(assignment2);
    std::cout << "Execution time: " << timer.elapsedMs() << " ms" << std::endl;
}

// ============================================================================
// ГЛАВНАЯ ФУНКЦИЯ
// ============================================================================

int main()
{
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║       GRAPH ALGORITHMS - COMPREHENSIVE TEST SUITE                    ║" << std::endl;
    std::cout << "║  Undirected Graph, Shortest Path, TSP, MST, Connectivity, Coloring  ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════╝" << std::endl;

    try
    {
        // Запускаем все тесты
        testGraphBasics();
        testShortestPath();
        testTSP();
        testSpanningTree();
        testConnectivity();
        testColoring();
        testHotelOptimization();

        std::cout << "\n"
                  << std::string(70, '=') << std::endl;
        std::cout << "ALL TESTS COMPLETED SUCCESSFULLY!" << std::endl;
        std::cout << std::string(70, '=') << std::endl
                  << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    // BENCHMARKS
    std::cout << "\n\n";
    Benchmark::runAllBenchmarks();
    // UNIT TESTS
    std::cout << "\n\n";
    int graphResult = runGraphTests();
    int algoResult = runAlgorithmTests();
    return 0;
}