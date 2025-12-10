#ifndef COLORING_H
#define COLORING_H

#include "../graph/graph.h"
#include <vector>
#include <iostream>
#include <set>

/**
 * Класс для раскраски графа (Graph Coloring Problem)
 */
template <typename Weight = double>
class GraphColorer
{
public:
    struct ColoringResult
    {
        std::vector<int> coloring; // coloring[v] = цвет вершины v
        int chromaticNumber;       // Минимальное количество цветов
        bool isValid;              // Валидна ли раскраска
    };

    /**
     * Жадный алгоритм раскраски (Greedy Coloring)
     * Не гарантирует минимальное количество цветов, но работает быстро
     * Сложность: O(V^2 + E)
     */
    static ColoringResult greedyColoring(const UndirectedGraph<Weight> &graph)
    {
        ColoringResult result;

        std::vector<int> vertices = graph.getAllVertices();
        result.coloring.assign(10000, -1);

        if (vertices.empty())
        {
            result.chromaticNumber = 0;
            result.isValid = true;
            return result;
        }

        // Первой вершине присваиваем цвет 0
        result.coloring[vertices[0]] = 0;

        // Вспомогательный массив для отслеживания доступных цветов
        std::vector<bool> available(vertices.size(), false);

        // Присваиваем цвета остальным вершинам
        for (int i = 1; i < vertices.size(); ++i)
        {
            int v = vertices[i];

            // Сбрасываем доступность цветов
            std::fill(available.begin(), available.end(), false);

            // Отмечаем цвета соседей как недоступные
            for (int neighbor : graph.getAdjacentVertices(v))
            {
                if (result.coloring[neighbor] != -1)
                {
                    available[result.coloring[neighbor]] = true;
                }
            }

            // Находим первый доступный цвет
            int color = 0;
            while (color < available.size() && available[color])
            {
                color++;
            }

            result.coloring[v] = color;
        }

        // Вычисляем хроматическое число
        result.chromaticNumber = 0;
        for (int v : vertices)
        {
            result.chromaticNumber = std::max(result.chromaticNumber, result.coloring[v]);
        }
        result.chromaticNumber++; // Цвета начинаются с 0

        result.isValid = validateColoring(graph, result);

        return result;
    }

    /**
     * Оптимизированный жадный алгоритм (Welsh-Powell)
     * Сортирует вершины по степени (количество соседей) в убывающем порядке
     * Часто дает лучший результат, чем простой жадный алгоритм
     * Сложность: O(V log V + V^2 + E)
     */
    static ColoringResult welshPowellColoring(const UndirectedGraph<Weight> &graph)
    {
        ColoringResult result;

        std::vector<int> vertices = graph.getAllVertices();
        result.coloring.assign(10000, -1);

        if (vertices.empty())
        {
            result.chromaticNumber = 0;
            result.isValid = true;
            return result;
        }

        // Сортируем вершины по степени в убывающем порядке
        std::sort(vertices.begin(), vertices.end(),
                  [&graph](int a, int b)
                  {
                      return graph.getAdjacentVertices(a).size() >
                             graph.getAdjacentVertices(b).size();
                  });

        // Вспомогательный массив для отслеживания доступных цветов
        std::vector<bool> available(vertices.size(), false);

        // Раскрашиваем вершины
        for (int v : vertices)
        {
            // Сбрасываем доступность цветов
            std::fill(available.begin(), available.end(), false);

            // Отмечаем цвета соседей как недоступные
            for (int neighbor : graph.getAdjacentVertices(v))
            {
                if (result.coloring[neighbor] != -1)
                {
                    available[result.coloring[neighbor]] = true;
                }
            }

            // Находим первый доступный цвет
            int color = 0;
            while (color < available.size() && available[color])
            {
                color++;
            }

            result.coloring[v] = color;
        }

        // Вычисляем хроматическое число
        result.chromaticNumber = 0;
        for (int v : vertices)
        {
            result.chromaticNumber = std::max(result.chromaticNumber, result.coloring[v]);
        }
        result.chromaticNumber++;

        result.isValid = validateColoring(graph, result);

        return result;
    }

    /**
     * Проверить валидность раскраски
     */
    static bool validateColoring(const UndirectedGraph<Weight> &graph, const ColoringResult &result)
    {
        std::vector<int> vertices = graph.getAllVertices();

        for (int v : vertices)
        {
            for (int neighbor : graph.getAdjacentVertices(v))
            {
                if (result.coloring[v] == result.coloring[neighbor])
                {
                    return false; // Два соседа имеют одинаковый цвет
                }
            }
        }

        return true;
    }

    /**
     * Вывести результат раскраски
     */
    static void printColoring(const UndirectedGraph<Weight> &graph, const ColoringResult &result)
    {
        std::cout << "=== Graph Coloring ===" << std::endl;
        std::cout << "Chromatic number: " << result.chromaticNumber << std::endl;
        std::cout << "Valid coloring: " << (result.isValid ? "Yes" : "No (ERROR!)") << std::endl;
        std::cout << "Vertex coloring:" << std::endl;

        std::vector<int> vertices = graph.getAllVertices();
        for (int v : vertices)
        {
            std::cout << "  Vertex " << v << " -> Color " << result.coloring[v] << std::endl;
        }

        // Показываем группы вершин по цветам
        std::cout << "Color groups:" << std::endl;
        std::set<int> usedColors;
        for (int v : vertices)
        {
            usedColors.insert(result.coloring[v]);
        }

        for (int color : usedColors)
        {
            std::cout << "  Color " << color << ": ";
            for (int v : vertices)
            {
                if (result.coloring[v] == color)
                {
                    std::cout << v << " ";
                }
            }
            std::cout << std::endl;
        }
    }
};

#endif // COLORING_H