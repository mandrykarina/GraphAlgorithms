#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

// ============================================================================
// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ И ФУНКЦИИ ДЛЯ ТЕСТОВ
// ============================================================================

int passedTests = 0;
int failedTests = 0;

inline double abs_diff(double a, double b)
{
    return std::abs(a - b);
}

inline void assertEqual(bool condition, const std::string &testName)
{
    if (condition)
    {
        passedTests++;
        std::cout << "  ✓ " << testName << std::endl;
    }
    else
    {
        failedTests++;
        std::cout << "  ✗ FAILED: " << testName << std::endl;
    }
}

// ============================================================================
// ВКЛЮЧЕНИЕ ЗАГОЛОВКОВ ГРАФОВ И АЛГОРИТМОВ
// ============================================================================

#include "graph/graph.h"
#include "algorithms/shortest_path.h"
#include "algorithms/tsp.h"
#include "algorithms/spanning_tree.h"
#include "algorithms/connectivity.h"
#include "algorithms/coloring.h"
#include "algorithms/hotel_optimization.h"

using Graph = UndirectedGraph<double>;
using ShortestPath = ShortestPathFinder<double>;
using TSP = TSPSolver<double>;
using MST = SpanningTreeFinder<double>;
using Connectivity = ConnectivityFinder<double>;
using Coloring = GraphColorer<double>;
using Hotel = HotelOptimization<double>;

// ============================================================================
// ТЕСТЫ DIJKSTRA
// ============================================================================

void testDijkstraSimple()
{
    std::cout << "\nTest: Dijkstra simple path" << std::endl;
    Graph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, 1, 2.0);
    g.addEdge(1, 2, 3.0);
    g.addEdge(0, 2, 5.0);

    auto path = ShortestPath::findPath(g, 0, 2);
    assertEqual(path.found == true, "path found");
    assertEqual(abs_diff(path.distance, 5.0) < 0.01, "distance is 5.0");
    assertEqual(path.path.size() == 3, "path has 3 vertices");
    assertEqual(path.path[0] == 0 && path.path[2] == 2, "correct start and end");
}

void testDijkstraNoPath()
{
    std::cout << "\nTest: Dijkstra no path" << std::endl;
    Graph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addVertex(3);
    g.addEdge(0, 1, 1.0);
    g.addEdge(2, 3, 1.0);

    auto path = ShortestPath::findPath(g, 0, 3);
    assertEqual(path.found == false, "no path found between components");
}

void testDijkstraAllPaths()
{
    std::cout << "\nTest: Dijkstra all paths" << std::endl;
    Graph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addVertex(3);
    g.addEdge(0, 1, 1.0);
    g.addEdge(1, 2, 2.0);
    g.addEdge(2, 3, 3.0);

    auto paths = ShortestPath::dijkstra(g, 0);
    assertEqual(abs_diff(paths.distances[0], 0.0) < 0.01, "distance to 0 is 0");
    assertEqual(abs_diff(paths.distances[1], 1.0) < 0.01, "distance to 1 is 1");
    assertEqual(abs_diff(paths.distances[2], 3.0) < 0.01, "distance to 2 is 3");
    assertEqual(abs_diff(paths.distances[3], 6.0) < 0.01, "distance to 3 is 6");
}

// ============================================================================
// ТЕСТЫ BFS
// ============================================================================

void testBFSSimple()
{
    std::cout << "\nTest: BFS simple path" << std::endl;
    Graph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, 1, 1.0);
    g.addEdge(1, 2, 1.0);

    auto path = ShortestPath::bfs(g, 0, 2);
    assertEqual(path.found == true, "path found");
    assertEqual(path.path.size() == 3, "path length is 3");
    assertEqual(path.distance == 2, "distance (hops) is 2");
}

// ============================================================================
// ТЕСТЫ TSP
// ============================================================================

void testTSPBruteForce()
{
    std::cout << "\nTest: TSP Brute Force" << std::endl;
    Graph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, 1, 1.0);
    g.addEdge(0, 2, 2.0);
    g.addEdge(1, 2, 1.0);

    auto result = TSP::bruteForce(g, 0);
    assertEqual(abs_diff(result.totalDistance, 4.0) < 0.01, "optimal distance is 4.0");
    assertEqual(result.isOptimal == true, "result is optimal");
    assertEqual(result.tour.size() == 4, "tour returns to start");
}

void testTSPNearestNeighbor()
{
    std::cout << "\nTest: TSP Nearest Neighbor" << std::endl;
    Graph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, 1, 1.0);
    g.addEdge(0, 2, 2.0);
    g.addEdge(1, 2, 1.0);

    auto result = TSP::nearestNeighbor(g, 0);
    assertEqual(result.totalDistance > 0, "distance calculated");
    assertEqual(result.tour.size() == 4, "tour returns to start");
    assertEqual(result.tour[0] == 0 && result.tour[3] == 0, "tour starts and ends at 0");
}

void testTSPHybrid()
{
    std::cout << "\nTest: TSP Hybrid (NN + 2-opt)" << std::endl;
    Graph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addVertex(3);
    g.addEdge(0, 1, 1.0);
    g.addEdge(1, 2, 1.0);
    g.addEdge(2, 3, 1.0);
    g.addEdge(3, 0, 1.0);
    g.addEdge(0, 2, 10.0);
    g.addEdge(1, 3, 10.0);

    auto result = TSP::hybridSolver(g, 0);
    assertEqual(result.totalDistance > 0, "distance calculated");
    assertEqual(result.tour[0] == 0 && result.tour[3] == 0, "valid tour");
}

// ============================================================================
// ТЕСТЫ MST
// ============================================================================

void testKruskal()
{
    std::cout << "\nTest: MST Kruskal" << std::endl;
    Graph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, 1, 1.0);
    g.addEdge(1, 2, 2.0);
    g.addEdge(0, 2, 10.0);

    auto mst = MST::kruskal(g);
    assertEqual(mst.isConnected == true, "MST is connected");
    assertEqual(mst.edges.size() == 2, "MST has 2 edges (V-1)");
    assertEqual(abs_diff(mst.totalWeight, 3.0) < 0.01, "total weight is 3.0");
}

void testPrim()
{
    std::cout << "\nTest: MST Prim" << std::endl;
    Graph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, 1, 1.0);
    g.addEdge(1, 2, 2.0);
    g.addEdge(0, 2, 10.0);

    auto mst = MST::prim(g, 0);
    assertEqual(mst.isConnected == true, "MST is connected");
    assertEqual(mst.edges.size() == 2, "MST has 2 edges");
    assertEqual(abs_diff(mst.totalWeight, 3.0) < 0.01, "total weight is 3.0");
}

// ============================================================================
// ТЕСТЫ CONNECTIVITY
// ============================================================================

void testConnectivityConnected()
{
    std::cout << "\nTest: Connectivity connected graph" << std::endl;
    Graph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, 1, 1.0);
    g.addEdge(1, 2, 1.0);

    auto conn = Connectivity::dfsFindComponents(g);
    assertEqual(conn.componentCount == 1, "1 component");
    assertEqual(Connectivity::isConnected(g) == true, "graph is connected");
}

void testConnectivityDisconnected()
{
    std::cout << "\nTest: Connectivity disconnected graph" << std::endl;
    Graph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addEdge(0, 1, 1.0);
    g.addVertex(2);
    g.addVertex(3);
    g.addEdge(2, 3, 1.0);
    g.addVertex(4);

    auto conn = Connectivity::dfsFindComponents(g);
    assertEqual(conn.componentCount == 3, "3 components");
    assertEqual(Connectivity::isConnected(g) == false, "graph is not connected");
    assertEqual(Connectivity::getLargestComponentSize(g) == 2, "largest component has 2 vertices");
}

void testConnectivityBFS()
{
    std::cout << "\nTest: Connectivity BFS" << std::endl;
    Graph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, 1, 1.0);
    g.addEdge(1, 2, 1.0);

    auto conn = Connectivity::bfsFindComponents(g);
    assertEqual(conn.componentCount == 1, "BFS finds 1 component");
}

// ============================================================================
// ТЕСТЫ COLORING
// ============================================================================

void testColoringGreedy()
{
    std::cout << "\nTest: Graph Coloring Greedy" << std::endl;
    Graph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, 1, 1.0);
    g.addEdge(1, 2, 1.0);
    g.addEdge(0, 2, 1.0);

    auto coloring = Coloring::greedyColoring(g);
    assertEqual(coloring.isValid == true, "coloring is valid");
    assertEqual(coloring.chromaticNumber <= 3, "at most 3 colors");

    bool valid = true;
    for (int v : g.getAllVertices())
    {
        for (int neighbor : g.getAdjacentVertices(v))
        {
            if (coloring.coloring[v] == coloring.coloring[neighbor])
            {
                valid = false;
            }
        }
    }
    assertEqual(valid, "neighbors have different colors");
}

void testColoringWelshPowell()
{
    std::cout << "\nTest: Graph Coloring Welsh-Powell" << std::endl;
    Graph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, 1, 1.0);
    g.addEdge(1, 2, 1.0);
    g.addEdge(0, 2, 1.0);

    auto coloring = Coloring::welshPowellColoring(g);
    assertEqual(coloring.isValid == true, "coloring is valid");
    assertEqual(coloring.chromaticNumber <= 3, "at most 3 colors");
}

// ============================================================================
// ТЕСТЫ HOTEL OPTIMIZATION
// ============================================================================

void testDominatingSet()
{
    std::cout << "\nTest: Hotel Optimization Dominating Set" << std::endl;
    Graph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addVertex(3);
    g.addEdge(0, 1, 1.0);
    g.addEdge(0, 2, 1.0);
    g.addEdge(0, 3, 1.0);

    auto result = Hotel::dominatingSetGreedy(g);
    assertEqual(result.isValid == true, "solution is valid");
    assertEqual(result.hotelRooms.size() <= 2, "at most 2 centers needed");
}

void testKCenters()
{
    std::cout << "\nTest: Hotel Optimization K-Centers" << std::endl;
    Graph g;
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addVertex(3);
    g.addEdge(0, 1, 1.0);
    g.addEdge(1, 2, 1.0);
    g.addEdge(2, 3, 1.0);

    auto result = Hotel::kCenters(g, 2);
    assertEqual(result.isValid == true, "solution is valid");
    assertEqual(result.hotelRooms.size() == 2, "exactly 2 centers");
}

// ============================================================================
// ГЛАВНАЯ ФУНКЦИЯ
// ============================================================================

int main()
{
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║       ALGORITHM UNIT TESTS - STANDALONE VERSION           ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;

    passedTests = 0;
    failedTests = 0;

    std::cout << "\n--- Running Dijkstra Tests ---" << std::endl;
    testDijkstraSimple();
    testDijkstraNoPath();
    testDijkstraAllPaths();

    std::cout << "\n--- Running BFS Tests ---" << std::endl;
    testBFSSimple();

    std::cout << "\n--- Running TSP Tests ---" << std::endl;
    testTSPBruteForce();
    testTSPNearestNeighbor();
    testTSPHybrid();

    std::cout << "\n--- Running MST Tests ---" << std::endl;
    testKruskal();
    testPrim();

    std::cout << "\n--- Running Connectivity Tests ---" << std::endl;
    testConnectivityConnected();
    testConnectivityDisconnected();
    testConnectivityBFS();

    std::cout << "\n--- Running Coloring Tests ---" << std::endl;
    testColoringGreedy();
    testColoringWelshPowell();

    std::cout << "\n--- Running Hotel Optimization Tests ---" << std::endl;
    testDominatingSet();
    testKCenters();

    // Результаты
    std::cout << "\n"
              << std::string(60, '=') << std::endl;
    std::cout << "FINAL RESULTS: " << passedTests << " PASSED, "
              << failedTests << " FAILED" << std::endl;
    std::cout << std::string(60, '=') << std::endl
              << std::endl;

    return failedTests == 0 ? 0 : 1;
}