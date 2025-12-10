#ifndef SHORTEST_PATH_H
#define SHORTEST_PATH_H

#include "../graph/graph.h"
#include <vector>
#include <limits>
#include <queue>
#include <algorithm>
#include <iostream>

/**
 * Класс для поиска кратчайшего пути в графе
 */
template <typename Weight = double>
class ShortestPathFinder
{
public:
    struct PathResult
    {
        std::vector<int> path; // Путь от source до target
        Weight distance;       // Общее расстояние
        bool found;            // Был ли найден путь
    };

    struct AllPaths
    {
        std::vector<Weight> distances; // Расстояния до каждой вершины
        std::vector<int> previous;     // Для восстановления пути
    };

    /**
     * Алгоритм Дейкстры - поиск кратчайшего пути от одной вершины до всех остальных
     * Сложность: O((V + E) log V)
     */
    static AllPaths dijkstra(const UndirectedGraph<Weight> &graph, int source)
    {
        int n = graph.getVertexCount();
        AllPaths result;
        result.distances.assign(10000, std::numeric_limits<Weight>::max());
        result.previous.assign(10000, -1);

        // Используем приоритетную очередь: (расстояние, вершина)
        std::priority_queue<std::pair<Weight, int>,
                            std::vector<std::pair<Weight, int>>,
                            std::greater<std::pair<Weight, int>>>
            pq;

        result.distances[source] = 0;
        pq.push({0, source});

        while (!pq.empty())
        {
            auto [dist, u] = pq.top();
            pq.pop();

            if (dist > result.distances[u])
                continue;

            // Проверяем всех соседей
            const auto &neighbors = graph.getAdjacencyList(u);
            for (const auto &[v, weight] : neighbors)
            {
                Weight newDist = result.distances[u] + weight;

                if (newDist < result.distances[v])
                {
                    result.distances[v] = newDist;
                    result.previous[v] = u;
                    pq.push({newDist, v});
                }
            }
        }

        return result;
    }

    /**
     * Найти путь от source до target используя алгоритм Дейкстры
     */
    static PathResult findPath(const UndirectedGraph<Weight> &graph, int source, int target)
    {
        PathResult result;
        result.found = false;
        result.distance = std::numeric_limits<Weight>::max();

        if (!graph.hasVertex(source) || !graph.hasVertex(target))
        {
            return result;
        }

        AllPaths paths = dijkstra(graph, source);

        if (paths.distances[target] == std::numeric_limits<Weight>::max())
        {
            return result; // Пути нет
        }

        // Восстанавливаем путь
        int current = target;
        while (current != -1)
        {
            result.path.push_back(current);
            current = paths.previous[current];
        }

        std::reverse(result.path.begin(), result.path.end());
        result.distance = paths.distances[target];
        result.found = true;

        return result;
    }

    /**
     * Алгоритм BFS - поиск кратчайшего пути в невзвешенном графе
     * Сложность: O(V + E)
     */
    static PathResult bfs(const UndirectedGraph<Weight> &graph, int source, int target)
    {
        PathResult result;
        result.found = false;
        result.distance = 0;

        if (!graph.hasVertex(source) || !graph.hasVertex(target))
        {
            return result;
        }

        std::vector<bool> visited(10000, false);
        std::vector<int> parent(10000, -1);
        std::queue<int> q;

        q.push(source);
        visited[source] = true;

        while (!q.empty())
        {
            int u = q.front();
            q.pop();

            if (u == target)
                break;

            for (int v : graph.getAdjacentVertices(u))
            {
                if (!visited[v])
                {
                    visited[v] = true;
                    parent[v] = u;
                    q.push(v);
                }
            }
        }

        if (!visited[target])
        {
            return result;
        }

        // Восстанавливаем путь
        int current = target;
        while (current != -1)
        {
            result.path.push_back(current);
            current = parent[current];
        }

        std::reverse(result.path.begin(), result.path.end());
        result.found = true;
        result.distance = result.path.size() - 1; // Количество ребер

        return result;
    }

    /**
     * Вывести результат поиска пути
     */
    static void printPath(const PathResult &result)
    {
        if (!result.found)
        {
            std::cout << "Path not found!" << std::endl;
            return;
        }

        std::cout << "Path found (distance: " << result.distance << "): ";
        for (int i = 0; i < result.path.size(); ++i)
        {
            if (i > 0)
                std::cout << " -> ";
            std::cout << result.path[i];
        }
        std::cout << std::endl;
    }
};

#endif // SHORTEST_PATH_H