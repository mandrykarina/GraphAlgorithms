#ifndef EDGE_H
#define EDGE_H

/**
 * Структура ребра графа
 */
template <typename Weight = double>
struct Edge
{
    int from;      // Вершина-источник
    int to;        // Вершина-приемник
    Weight weight; // Вес ребра

    Edge() : from(-1), to(-1), weight(0) {}
    Edge(int from, int to, Weight weight)
        : from(from), to(to), weight(weight) {}

    // Оператор для сортировки по весу
    bool operator<(const Edge &other) const
    {
        return weight < other.weight;
    }

    bool operator>(const Edge &other) const
    {
        return weight > other.weight;
    }
};

#endif // EDGE_H