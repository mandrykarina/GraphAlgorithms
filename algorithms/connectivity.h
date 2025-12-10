#ifndef CONNECTIVITY_H
#define CONNECTIVITY_H

#include "../graph/graph.h"
#include <vector>
#include <queue>
#include <iostream>
#include <set>

/**
 * Класс для поиска компонент связности в неориентированном графе
 */
template <typename Weight = double>
class ConnectivityFinder
{
public:
    struct ConnectivityResult
    {
        std::vector<std::vector<int>> components; // Каждая компонента - набор вершин
        int componentCount;                       // Количество компонент
        std::vector<int> componentId;             // componentId[v] = ID компоненты для вершины v
    };

    /**
     * Поиск компонент связности используя DFS (поиск в глубину)
     * Сложность: O(V + E)
     */
    static ConnectivityResult dfsFindComponents(const UndirectedGraph<Weight> &graph)
    {
        ConnectivityResult result;

        int n = graph.getVertexCount();
        result.componentId.assign(10000, -1);
        result.componentCount = 0;

        std::vector<int> vertices = graph.getAllVertices();

        for (int v : vertices)
        {
            if (result.componentId[v] == -1)
            {
                // Начинаем новую компоненту
                std::vector<int> component;
                dfsHelper(graph, v, result.componentCount, component, result.componentId);
                result.components.push_back(component);
                result.componentCount++;
            }
        }

        return result;
    }

    /**
     * Поиск компонент связности используя BFS (поиск в ширину)
     * Сложность: O(V + E)
     */
    static ConnectivityResult bfsFindComponents(const UndirectedGraph<Weight> &graph)
    {
        ConnectivityResult result;

        int n = graph.getVertexCount();
        result.componentId.assign(10000, -1);
        result.componentCount = 0;

        std::vector<int> vertices = graph.getAllVertices();

        for (int start : vertices)
        {
            if (result.componentId[start] == -1)
            {
                // Начинаем новую компоненту
                std::vector<int> component;
                bfsHelper(graph, start, result.componentCount, component, result.componentId);
                result.components.push_back(component);
                result.componentCount++;
            }
        }

        return result;
    }

    /**
     * Проверить, связан ли граф
     */
    static bool isConnected(const UndirectedGraph<Weight> &graph)
    {
        auto result = dfsFindComponents(graph);
        return result.componentCount <= 1;
    }

    /**
     * Получить размер наибольшей компоненты
     */
    static int getLargestComponentSize(const UndirectedGraph<Weight> &graph)
    {
        auto result = dfsFindComponents(graph);

        int maxSize = 0;
        for (const auto &component : result.components)
        {
            maxSize = std::max(maxSize, (int)component.size());
        }

        return maxSize;
    }

    /**
     * Вывести результат
     */
    static void printConnectivity(const ConnectivityResult &result)
    {
        std::cout << "=== Connectivity Components ===" << std::endl;
        std::cout << "Number of components: " << result.componentCount << std::endl;

        for (int i = 0; i < result.components.size(); ++i)
        {
            std::cout << "Component " << i << " (size: " << result.components[i].size() << "): ";
            for (int v : result.components[i])
            {
                std::cout << v << " ";
            }
            std::cout << std::endl;
        }
    }

private:
    /**
     * Вспомогательная функция для DFS
     */
    static void dfsHelper(const UndirectedGraph<Weight> &graph, int v, int compId,
                          std::vector<int> &component, std::vector<int> &componentId)
    {
        componentId[v] = compId;
        component.push_back(v);

        for (int neighbor : graph.getAdjacentVertices(v))
        {
            if (componentId[neighbor] == -1)
            {
                dfsHelper(graph, neighbor, compId, component, componentId);
            }
        }
    }

    /**
     * Вспомогательная функция для BFS
     */
    static void bfsHelper(const UndirectedGraph<Weight> &graph, int start, int compId,
                          std::vector<int> &component, std::vector<int> &componentId)
    {
        std::queue<int> q;
        q.push(start);
        componentId[start] = compId;

        while (!q.empty())
        {
            int v = q.front();
            q.pop();
            component.push_back(v);

            for (int neighbor : graph.getAdjacentVertices(v))
            {
                if (componentId[neighbor] == -1)
                {
                    componentId[neighbor] = compId;
                    q.push(neighbor);
                }
            }
        }
    }
};

#endif // CONNECTIVITY_H