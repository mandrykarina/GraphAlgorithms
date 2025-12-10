#ifndef SPANNING_TREE_H
#define SPANNING_TREE_H

#include "../graph/graph.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>

/**
 * Union-Find структура данных для алгоритма Кракала
 */
class UnionFind
{
private:
    std::vector<int> parent;
    std::vector<int> rank;

public:
    UnionFind(int n) : parent(n), rank(n, 0)
    {
        std::iota(parent.begin(), parent.end(), 0);
    }

    int find(int x)
    {
        if (parent[x] != x)
        {
            parent[x] = find(parent[x]); // Сжатие пути
        }
        return parent[x];
    }

    bool unite(int x, int y)
    {
        int px = find(x);
        int py = find(y);

        if (px == py)
            return false;

        // Объединение по рангу
        if (rank[px] < rank[py])
        {
            parent[px] = py;
        }
        else if (rank[px] > rank[py])
        {
            parent[py] = px;
        }
        else
        {
            parent[py] = px;
            rank[px]++;
        }

        return true;
    }
};

/**
 * Класс для поиска минимального остовного дерева (Minimum Spanning Tree)
 */
template <typename Weight = double>
class SpanningTreeFinder
{
public:
    struct MSTResult
    {
        std::vector<Edge<Weight>> edges; // Ребра остова
        Weight totalWeight;              // Общий вес
        int vertexCount;                 // Количество вершин
        bool isConnected;                // Связан ли граф
    };

    /**
     * Алгоритм Кракала - поиск MST
     * Сложность: O(E log E)
     * Основан на сортировке ребер и Union-Find структуре
     */
    static MSTResult kruskal(const UndirectedGraph<Weight> &graph)
    {
        MSTResult result;
        result.vertexCount = graph.getVertexCount();
        result.totalWeight = 0;
        result.isConnected = false;

        if (graph.getVertexCount() == 0)
        {
            return result;
        }

        // Получаем все ребра и сортируем их по весу
        std::vector<Edge<Weight>> edges = graph.getAllEdges();
        std::sort(edges.begin(), edges.end());

        // Union-Find структура для проверки циклов
        UnionFind uf(10000);

        // Добавляем ребра в порядке возрастания веса
        for (const auto &edge : edges)
        {
            if (uf.unite(edge.from, edge.to))
            {
                result.edges.push_back(edge);
                result.totalWeight += edge.weight;

                // MST содержит V-1 ребер
                if (result.edges.size() == graph.getVertexCount() - 1)
                {
                    result.isConnected = true;
                    break;
                }
            }
        }

        return result;
    }

    /**
     * Алгоритм Прима - поиск MST
     * Сложность: O(V^2) при использовании матрицы смежности, O((V + E) log V) с приоритетной очередью
     * Начинает с одной вершины и последовательно добавляет ребра
     */
    static MSTResult prim(const UndirectedGraph<Weight> &graph, int startVertex = 0)
    {
        MSTResult result;
        result.vertexCount = graph.getVertexCount();
        result.totalWeight = 0;
        result.isConnected = false;

        if (graph.getVertexCount() == 0 || !graph.hasVertex(startVertex))
        {
            return result;
        }

        std::vector<bool> inMST(10000, false);
        std::vector<Weight> minCost(10000, std::numeric_limits<Weight>::max());
        std::vector<int> parent(10000, -1);

        // Начинаем с первой вершины
        minCost[startVertex] = 0;

        // Добавляем V вершин в MST
        for (int count = 0; count < graph.getVertexCount(); ++count)
        {
            // Ищем вершину с минимальной стоимостью среди не добавленных
            int u = -1;
            Weight minVal = std::numeric_limits<Weight>::max();

            for (int v : graph.getAllVertices())
            {
                if (!inMST[v] && minCost[v] < minVal)
                {
                    minVal = minCost[v];
                    u = v;
                }
            }

            if (u == -1)
                break; // Граф несвязный

            inMST[u] = true;

            // Добавляем ребро в MST
            if (parent[u] != -1)
            {
                result.edges.push_back({parent[u], u, minCost[u]});
                result.totalWeight += minCost[u];
            }

            // Обновляем стоимости соседних вершин
            for (const auto &[v, weight] : graph.getAdjacencyList(u))
            {
                if (!inMST[v] && weight < minCost[v])
                {
                    minCost[v] = weight;
                    parent[v] = u;
                }
            }
        }

        result.isConnected = (result.edges.size() == graph.getVertexCount() - 1);

        return result;
    }

    /**
     * Вывести результат MST
     */
    static void printMST(const MSTResult &result)
    {
        std::cout << "=== Minimum Spanning Tree ===" << std::endl;
        std::cout << "Vertices: " << result.vertexCount << std::endl;
        std::cout << "Edges in MST: " << result.edges.size() << std::endl;
        std::cout << "Total weight: " << result.totalWeight << std::endl;
        std::cout << "Connected: " << (result.isConnected ? "Yes" : "No (graph is disconnected)") << std::endl;
        std::cout << "Edges:" << std::endl;

        for (const auto &edge : result.edges)
        {
            std::cout << "  " << edge.from << " - " << edge.to
                      << " (weight: " << edge.weight << ")" << std::endl;
        }
    }
};

#endif // SPANNING_TREE_H