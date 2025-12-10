#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
// ИСПРАВЛЕНЫ ПУТИ - используем относительные пути от корневой папки
#include "graph/graph.h"
#include "algorithms/shortest_path.h"
#include "algorithms/tsp.h"
#include "algorithms/spanning_tree.h"
#include "algorithms/connectivity.h"
#include "algorithms/coloring.h"
#include "utils/timer.h"

using Graph = UndirectedGraph<double>;
using ShortestPath = ShortestPathFinder<double>;
using TSP = TSPSolver<double>;
using MST = SpanningTreeFinder<double>;
using Connectivity = ConnectivityFinder<double>;
using Coloring = GraphColorer<double>;

/**
 * Класс для тестирования производительности алгоритмов
 */
class Benchmark
{
public:
    /**
     * Создать случайный граф размером n вершин
     */
    static Graph createRandomGraph(int n, double edgeDensity = 0.5)
    {
        Graph g;

        // Добавляем вершины
        for (int i = 0; i < n; i++)
        {
            g.addVertex(i, "V" + std::to_string(i));
        }

        // Добавляем ребра случайно
        for (int i = 0; i < n; i++)
        {
            for (int j = i + 1; j < n; j++)
            {
                if ((rand() / (double)RAND_MAX) < edgeDensity)
                {
                    double weight = 1.0 + (rand() % 100) / 10.0;
                    g.addEdge(i, j, weight);
                }
            }
        }

        return g;
    }

    /**
     * Создать полный граф
     */
    static Graph createCompleteGraph(int n)
    {
        Graph g;

        for (int i = 0; i < n; i++)
        {
            g.addVertex(i);
        }

        for (int i = 0; i < n; i++)
        {
            for (int j = i + 1; j < n; j++)
            {
                g.addEdge(i, j, 1.0 + (i + j) % 10);
            }
        }

        return g;
    }

    // ========================================================================
    // ТЕСТЫ SHORTEST PATH
    // ========================================================================

    static void benchmarkShortestPath()
    {
        std::cout << "\n"
                  << std::string(70, '=') << std::endl;
        std::cout << "BENCHMARK: Shortest Path Algorithms" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        std::cout << "\nGraph Size\tDijkstra(ms)\tBFS(ms)\tDijkstra/BFS" << std::endl;
        std::cout << std::string(60, '-') << std::endl;

        for (int n = 10; n <= 100; n += 10)
        {
            Graph g = createRandomGraph(n, 0.3);

            // Dijkstra
            Timer t1;
            t1.start();
            for (int i = 0; i < 5; i++)
            {
                ShortestPath::dijkstra(g, 0);
            }
            t1.stop();
            double dijkstraTime = t1.elapsedMs() / 5.0;

            // BFS
            Timer t2;
            t2.start();
            for (int i = 0; i < 5; i++)
            {
                ShortestPath::bfs(g, 0, n - 1);
            }
            t2.stop();
            double bfsTime = t2.elapsedMs() / 5.0;

            double ratio = dijkstraTime / (bfsTime > 0.001 ? bfsTime : 1);

            std::cout << n << " vertices\t"
                      << std::fixed << std::setprecision(3)
                      << dijkstraTime << "\t\t"
                      << bfsTime << "\t"
                      << ratio << "x" << std::endl;
        }
    }

    // ========================================================================
    // ТЕСТЫ TSP
    // ========================================================================

    static void benchmarkTSP()
    {
        std::cout << "\n"
                  << std::string(70, '=') << std::endl;
        std::cout << "BENCHMARK: TSP Algorithms (Complete Graphs)" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        std::cout << "\nGraph Size\tBrute(ms)\tNN(ms)\t2-opt(ms)\tNN+2opt(ms)" << std::endl;
        std::cout << std::string(70, '-') << std::endl;

        for (int n = 5; n <= 11; n++)
        {
            Graph g = createCompleteGraph(n);

            Timer t1, t2, t3, t4;
            double bruteTime = 0, nnTime = 0, twoOptTime = 0, hybridTime = 0;

            // Brute Force
            if (n <= 10)
            {
                t1.start();
                auto result = TSP::bruteForce(g, 0);
                t1.stop();
                bruteTime = t1.elapsedMs();
            }

            // Nearest Neighbor
            t2.start();
            auto nnResult = TSP::nearestNeighbor(g, 0);
            t2.stop();
            nnTime = t2.elapsedMs();

            // 2-opt
            t3.start();
            TSP::twoOpt(g, nnResult);
            t3.stop();
            twoOptTime = t3.elapsedMs();

            // Hybrid
            t4.start();
            TSP::hybridSolver(g, 0);
            t4.stop();
            hybridTime = t4.elapsedMs();

            std::cout << n << " cities\t";
            if (n <= 10)
            {
                std::cout << std::fixed << std::setprecision(3) << bruteTime << "\t";
            }
            else
            {
                std::cout << "N/A\t";
            }
            std::cout << std::fixed << std::setprecision(3)
                      << nnTime << "\t"
                      << twoOptTime << "\t"
                      << hybridTime << std::endl;
        }
    }

    // ========================================================================
    // ТЕСТЫ MST
    // ========================================================================

    static void benchmarkMST()
    {
        std::cout << "\n"
                  << std::string(70, '=') << std::endl;
        std::cout << "BENCHMARK: MST Algorithms" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        std::cout << "\nGraph Size\tKruskal(ms)\tPrim(ms)\tKruskal/Prim" << std::endl;
        std::cout << std::string(60, '-') << std::endl;

        for (int n = 10; n <= 100; n += 10)
        {
            Graph g = createRandomGraph(n, 0.5);

            // Kruskal
            Timer t1;
            t1.start();
            for (int i = 0; i < 10; i++)
            {
                MST::kruskal(g);
            }
            t1.stop();
            double kruskalTime = t1.elapsedMs() / 10.0;

            // Prim
            Timer t2;
            t2.start();
            for (int i = 0; i < 10; i++)
            {
                MST::prim(g, 0);
            }
            t2.stop();
            double primTime = t2.elapsedMs() / 10.0;

            double ratio = kruskalTime / (primTime > 0.001 ? primTime : 1);

            std::cout << n << " vertices\t"
                      << std::fixed << std::setprecision(3)
                      << kruskalTime << "\t\t"
                      << primTime << "\t"
                      << ratio << "x" << std::endl;
        }
    }

    // ========================================================================
    // ТЕСТЫ CONNECTIVITY
    // ========================================================================

    static void benchmarkConnectivity()
    {
        std::cout << "\n"
                  << std::string(70, '=') << std::endl;
        std::cout << "BENCHMARK: Connectivity Algorithms" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        std::cout << "\nGraph Size\tDFS(ms)\tBFS(ms)\tDFS/BFS" << std::endl;
        std::cout << std::string(50, '-') << std::endl;

        for (int n = 10; n <= 100; n += 10)
        {
            Graph g = createRandomGraph(n, 0.4);

            // DFS
            Timer t1;
            t1.start();
            for (int i = 0; i < 20; i++)
            {
                Connectivity::dfsFindComponents(g);
            }
            t1.stop();
            double dfsTime = t1.elapsedMs() / 20.0;

            // BFS
            Timer t2;
            t2.start();
            for (int i = 0; i < 20; i++)
            {
                Connectivity::bfsFindComponents(g);
            }
            t2.stop();
            double bfsTime = t2.elapsedMs() / 20.0;

            double ratio = dfsTime / (bfsTime > 0.001 ? bfsTime : 1);

            std::cout << n << " vertices\t"
                      << std::fixed << std::setprecision(3)
                      << dfsTime << "\t"
                      << bfsTime << "\t"
                      << ratio << "x" << std::endl;
        }
    }

    // ========================================================================
    // ТЕСТЫ COLORING
    // ========================================================================

    static void benchmarkColoring()
    {
        std::cout << "\n"
                  << std::string(70, '=') << std::endl;
        std::cout << "BENCHMARK: Graph Coloring Algorithms" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        std::cout << "\nGraph Size\tGreedy(ms)\tWelsh-Powell(ms)\tRatio" << std::endl;
        std::cout << std::string(65, '-') << std::endl;

        for (int n = 10; n <= 80; n += 10)
        {
            Graph g = createRandomGraph(n, 0.4);

            // Greedy
            Timer t1;
            t1.start();
            for (int i = 0; i < 10; i++)
            {
                Coloring::greedyColoring(g);
            }
            t1.stop();
            double greedyTime = t1.elapsedMs() / 10.0;

            // Welsh-Powell
            Timer t2;
            t2.start();
            for (int i = 0; i < 10; i++)
            {
                Coloring::welshPowellColoring(g);
            }
            t2.stop();
            double wpTime = t2.elapsedMs() / 10.0;

            double ratio = greedyTime / (wpTime > 0.001 ? wpTime : 1);

            std::cout << n << " vertices\t"
                      << std::fixed << std::setprecision(3)
                      << greedyTime << "\t\t"
                      << wpTime << "\t\t"
                      << ratio << "x" << std::endl;
        }
    }

    // ========================================================================
    // СРАВНЕНИЕ ВСЕХ АЛГОРИТМОВ
    // ========================================================================

    static void runAllBenchmarks()
    {
        std::cout << "\n\n";
        std::cout << "╔══════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    PERFORMANCE BENCHMARKS                            ║" << std::endl;
        std::cout << "║            Сравнение скорости всех реализованных алгоритмов          ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════════════════════════════════╝" << std::endl;

        benchmarkShortestPath();
        benchmarkTSP();
        benchmarkMST();
        benchmarkConnectivity();
        benchmarkColoring();

        std::cout << "\n"
                  << std::string(70, '=') << std::endl;
        std::cout << "BENCHMARKS COMPLETED" << std::endl;
        std::cout << std::string(70, '=') << std::endl
                  << std::endl;
    }
};

#endif // BENCHMARK_H