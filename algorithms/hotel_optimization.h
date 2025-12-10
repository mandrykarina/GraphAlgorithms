#ifndef HOTEL_OPTIMIZATION_H
#define HOTEL_OPTIMIZATION_H

#include "../graph/graph.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <set>

/**
 * Класс для решения задачи об оптимальном поселении в гостинице
 *
 * Задача: Найти минимальное множество комнат (вершин графа) такое,
 * что из каждой комнаты можно добраться до гостиницы за минимальное время
 * (т.е. нужно покрыть все вершины минимальным числом "центров"/комнат)
 */
template <typename Weight = double>
class HotelOptimization
{
public:
    struct RoomAssignment
    {
        std::vector<int> hotelRooms; // ID выбранных комнат (центров)
        std::vector<int> assignment; // assignment[v] = ID комнаты, к которой привязана комната v
        Weight maxDistance;          // Максимальное расстояние от любой комнаты до ее центра
        double averageDistance;      // Среднее расстояние
        bool isValid;
    };

    /**
     * Жадный алгоритм поиска доминирующего множества (Dominating Set)
     * Находит минимальный набор вершин, так чтобы каждая вершина либо
     * принадлежала этому набору, либо была смежна с вершиной из набора
     *
     * Сложность: O(V^2 + E)
     */
    static RoomAssignment dominatingSetGreedy(const UndirectedGraph<Weight> &graph)
    {
        RoomAssignment result;
        result.assignment.assign(10000, -1);
        result.maxDistance = 0;
        result.averageDistance = 0;
        result.isValid = false;

        std::vector<int> vertices = graph.getAllVertices();
        if (vertices.empty())
            return result;

        std::set<int> uncovered(vertices.begin(), vertices.end());

        while (!uncovered.empty())
        {
            // Выбираем вершину с максимальной степенью в подграфе покрытых вершин
            int best = -1;
            int bestScore = -1;

            for (int v : uncovered)
            {
                int score = 1; // Сама вершина
                for (int neighbor : graph.getAdjacentVertices(v))
                {
                    if (uncovered.count(neighbor))
                        score++;
                }

                if (score > bestScore)
                {
                    bestScore = score;
                    best = v;
                }
            }

            if (best == -1)
                break;

            result.hotelRooms.push_back(best);

            // Отмечаем best и его соседей как покрытые
            uncovered.erase(best);
            for (int neighbor : graph.getAdjacentVertices(best))
            {
                uncovered.erase(neighbor);
            }
        }

        // Назначаем каждую вершину ближайшему отелю
        assignToNearest(graph, result);

        return result;
    }

    /**
     * К-центров алгоритм (K-Centers Problem)
     * Находит k вершин (центров) такие, что максимальное расстояние от любой вершины до ближайшего центра минимально
     *
     * Сложность: O(V^3) - аппроксимационный алгоритм
     */
    static RoomAssignment kCenters(const UndirectedGraph<Weight> &graph, int k)
    {
        RoomAssignment result;
        result.assignment.assign(10000, -1);
        result.maxDistance = 0;
        result.isValid = false;

        std::vector<int> vertices = graph.getAllVertices();
        if (vertices.empty() || k <= 0 || k > vertices.size())
            return result;

        // Начинаем с вершины 0
        result.hotelRooms.push_back(vertices[0]);

        // Жадно выбираем оставшиеся k-1 центров
        for (int i = 1; i < k && i < vertices.size(); ++i)
        {
            int farthest = -1;
            Weight maxMinDist = -1;

            // Ищем вершину, которая максимально далеко от всех выбранных центров
            for (int v : vertices)
            {
                bool alreadyCenter = false;
                for (int center : result.hotelRooms)
                {
                    if (v == center)
                    {
                        alreadyCenter = true;
                        break;
                    }
                }

                if (alreadyCenter)
                    continue;

                // Находим минимальное расстояние от v до любого центра
                Weight minDist = std::numeric_limits<Weight>::max();
                for (int center : result.hotelRooms)
                {
                    // Используем BFS для поиска расстояния
                    Weight dist = findDistance(graph, v, center);
                    minDist = std::min(minDist, dist);
                }

                if (minDist > maxMinDist)
                {
                    maxMinDist = minDist;
                    farthest = v;
                }
            }

            if (farthest != -1)
            {
                result.hotelRooms.push_back(farthest);
            }
        }

        // Назначаем каждую вершину ближайшему центру
        assignToNearest(graph, result);

        return result;
    }

    /**
     * Вывести результат
     */
    static void printAssignment(const RoomAssignment &result)
    {
        std::cout << "=== Hotel Optimization ===" << std::endl;
        std::cout << "Number of main rooms (centers): " << result.hotelRooms.size() << std::endl;
        std::cout << "Main rooms: ";
        for (int room : result.hotelRooms)
        {
            std::cout << room << " ";
        }
        std::cout << std::endl;
        std::cout << "Max distance to nearest room: " << result.maxDistance << std::endl;
        std::cout << "Average distance: " << result.averageDistance << std::endl;
    }

private:
    /**
     * Назначить каждую вершину ближайшему центру
     */
    static void assignToNearest(const UndirectedGraph<Weight> &graph, RoomAssignment &result)
    {
        std::vector<int> vertices = graph.getAllVertices();
        Weight totalDist = 0;

        for (int v : vertices)
        {
            int nearest = result.hotelRooms[0];
            Weight minDist = findDistance(graph, v, nearest);

            for (int center : result.hotelRooms)
            {
                Weight dist = findDistance(graph, v, center);
                if (dist < minDist)
                {
                    minDist = dist;
                    nearest = center;
                }
            }

            result.assignment[v] = nearest;
            result.maxDistance = std::max(result.maxDistance, minDist);
            totalDist += minDist;
        }

        if (!vertices.empty())
        {
            result.averageDistance = totalDist / vertices.size();
        }

        result.isValid = true;
    }

    /**
     * Найти расстояние между двумя вершинами используя BFS
     */
    static Weight findDistance(const UndirectedGraph<Weight> &graph, int from, int to)
    {
        if (from == to)
            return 0;

        std::vector<bool> visited(10000, false);
        std::vector<Weight> dist(10000, std::numeric_limits<Weight>::max());

        std::queue<int> q;
        q.push(from);
        visited[from] = true;
        dist[from] = 0;

        while (!q.empty())
        {
            int u = q.front();
            q.pop();

            if (u == to)
                return dist[to];

            for (const auto &[v, weight] : graph.getAdjacencyList(u))
            {
                if (!visited[v])
                {
                    visited[v] = true;
                    dist[v] = dist[u] + weight;
                    q.push(v);
                }
            }
        }

        return std::numeric_limits<Weight>::max();
    }
};

#endif // HOTEL_OPTIMIZATION_H