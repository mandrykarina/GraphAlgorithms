#ifndef TSP_H
#define TSP_H

#include "../graph/graph.h"
#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>
#include <chrono>

/**
 * Решение задачи коммивояжера (Traveling Salesman Problem)
 */
template <typename Weight = double>
class TSPSolver
{
public:
    struct TSPResult
    {
        std::vector<int> tour; // Порядок посещения вершин
        Weight totalDistance;  // Общее расстояние тура
        int iterations;        // Количество проверенных маршрутов
        bool isOptimal;        // Является ли оптимальным (для полного перебора)
    };

    /**
     * Метод полного перебора (Brute Force) - гарантирует оптимальное решение
     * Сложность: O(n!)
     * Применяется только для небольших графов (n <= 12)
     */
    static TSPResult bruteForce(const UndirectedGraph<Weight> &graph, int startVertex)
    {
        TSPResult result;
        result.isOptimal = true;
        result.iterations = 0;

        std::vector<int> vertices = graph.getAllVertices();

        if (vertices.empty() || !graph.hasVertex(startVertex))
        {
            return result;
        }

        // Удаляем стартовую вершину из набора для перестановок
        auto it = std::find(vertices.begin(), vertices.end(), startVertex);
        if (it != vertices.end())
        {
            std::swap(*it, vertices.back());
            vertices.pop_back();
        }

        Weight minDistance = std::numeric_limits<Weight>::max();
        std::vector<int> bestTour;

        // Генерируем все перестановки
        std::sort(vertices.begin(), vertices.end());

        do
        {
            result.iterations++;

            Weight distance = 0;
            int current = startVertex;

            // Проходим по всем вершинам в текущей перестановке
            for (int next : vertices)
            {
                if (graph.hasEdge(current, next))
                {
                    distance += graph.getEdgeWeight(current, next);
                    current = next;
                }
                else
                {
                    distance = std::numeric_limits<Weight>::max();
                    break;
                }
            }

            // Возвращаемся в стартовую вершину
            if (distance != std::numeric_limits<Weight>::max() && graph.hasEdge(current, startVertex))
            {
                distance += graph.getEdgeWeight(current, startVertex);

                if (distance < minDistance)
                {
                    minDistance = distance;
                    bestTour = vertices;
                }
            }

        } while (std::next_permutation(vertices.begin(), vertices.end()));

        // Восстанавливаем полный тур
        result.tour.push_back(startVertex);
        for (int v : bestTour)
        {
            result.tour.push_back(v);
        }
        result.tour.push_back(startVertex); // Возвращаемся в начало

        result.totalDistance = minDistance;

        return result;
    }

    /**
     * Жадный алгоритм (Nearest Neighbor Heuristic)
     * Сложность: O(n^2)
     * Не гарантирует оптимальное решение, но быстро работает на больших графах
     */
    static TSPResult nearestNeighbor(const UndirectedGraph<Weight> &graph, int startVertex)
    {
        TSPResult result;
        result.isOptimal = false;
        result.iterations = 0;

        if (!graph.hasVertex(startVertex))
        {
            return result;
        }

        std::vector<bool> visited(10000, false);
        result.tour.push_back(startVertex);
        visited[startVertex] = true;

        int current = startVertex;
        int remaining = graph.getVertexCount() - 1;

        // Посещаем оставшиеся вершины
        while (remaining > 0)
        {
            result.iterations++;

            int nearest = -1;
            Weight minWeight = std::numeric_limits<Weight>::max();

            // Ищем ближайшую непосещенную вершину
            for (int v : graph.getAllVertices())
            {
                if (!visited[v] && graph.hasEdge(current, v))
                {
                    Weight w = graph.getEdgeWeight(current, v);
                    if (w < minWeight)
                    {
                        minWeight = w;
                        nearest = v;
                    }
                }
            }

            if (nearest == -1)
            {
                // Нет пути в оставшиеся вершины
                result.totalDistance = std::numeric_limits<Weight>::max();
                return result;
            }

            result.tour.push_back(nearest);
            visited[nearest] = true;
            result.totalDistance += minWeight;
            current = nearest;
            remaining--;
        }

        // Возвращаемся в стартовую вершину
        if (graph.hasEdge(current, startVertex))
        {
            result.totalDistance += graph.getEdgeWeight(current, startVertex);
            result.tour.push_back(startVertex);
        }
        else
        {
            result.totalDistance = std::numeric_limits<Weight>::max();
        }

        return result;
    }

    /**
     * 2-opt локальный поиск - улучшение существующего тура
     * Пытается улучшить тур путем обмена пар ребер
     */
    static TSPResult twoOpt(const UndirectedGraph<Weight> &graph, TSPResult initialTour)
    {
        TSPResult result = initialTour;
        bool improved = true;

        while (improved)
        {
            improved = false;

            for (int i = 1; i < result.tour.size() - 2; ++i)
            {
                for (int j = i + 1; j < result.tour.size() - 1; ++j)
                {
                    result.iterations++;

                    int a = result.tour[i - 1];
                    int b = result.tour[i];
                    int c = result.tour[j];
                    int d = result.tour[j + 1];

                    Weight oldDist = graph.getEdgeWeight(a, b) + graph.getEdgeWeight(c, d);
                    Weight newDist = graph.getEdgeWeight(a, c) + graph.getEdgeWeight(b, d);

                    if (newDist < oldDist)
                    {
                        // Развертываем последовательность между i и j
                        std::reverse(result.tour.begin() + i, result.tour.begin() + j + 1);
                        result.totalDistance -= (oldDist - newDist);
                        improved = true;
                    }
                }
            }
        }

        return result;
    }

    /**
     * Комбинированный подход: Nearest Neighbor + 2-opt улучшение
     */
    static TSPResult hybridSolver(const UndirectedGraph<Weight> &graph, int startVertex)
    {
        auto initial = nearestNeighbor(graph, startVertex);
        return twoOpt(graph, initial);
    }

    /**
     * Вывести результат TSP
     */
    static void printResult(const TSPResult &result)
    {
        std::cout << "=== TSP Solution ===" << std::endl;
        std::cout << "Total distance: " << result.totalDistance << std::endl;
        std::cout << "Iterations: " << result.iterations << std::endl;
        std::cout << "Optimal: " << (result.isOptimal ? "Yes" : "No (heuristic)") << std::endl;
        std::cout << "Tour: ";

        for (int i = 0; i < result.tour.size(); ++i)
        {
            if (i > 0)
                std::cout << " -> ";
            std::cout << result.tour[i];
        }
        std::cout << std::endl;
    }
};

#endif // TSP_H