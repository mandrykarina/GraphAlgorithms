#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <set>
#include <memory>
#include "vertex.h"
#include "edge.h"

/**
 * Класс неориентированного взвешенного графа
 * Реализация: список смежности
 */
template <typename Weight = double>
class UndirectedGraph
{
public:
    using EdgeType = Edge<Weight>;

    UndirectedGraph();
    ~UndirectedGraph() = default;

    // ===== Основные операции =====
    void addVertex(int id, const std::string &label = "");
    void removeVertex(int id);

    void addEdge(int from, int to, Weight weight);
    void removeEdge(int from, int to);

    bool hasVertex(int id) const;
    bool hasEdge(int from, int to) const;

    // ===== Запросы =====
    int getVertexCount() const { return vertices.size(); }
    int getEdgeCount() const { return edgeCount; }

    const Vertex &getVertex(int id) const;
    Weight getEdgeWeight(int from, int to) const;

    // ===== Соседи =====
    std::vector<int> getAdjacentVertices(int id) const;
    const std::vector<std::pair<int, Weight>> &getAdjacencyList(int id) const;

    // ===== Получение всех элементов =====
    std::vector<int> getAllVertices() const;
    std::vector<EdgeType> getAllEdges() const;

    // ===== Вспомогательное =====
    void clear();
    void printInfo() const;

protected:
    std::unordered_map<int, Vertex> vertices;                         // Вершины
    std::unordered_map<int, std::vector<std::pair<int, Weight>>> adj; // Список смежности
    int edgeCount = 0;
};

// ===== Реализация методов =====

template <typename Weight>
UndirectedGraph<Weight>::UndirectedGraph() : edgeCount(0) {}

template <typename Weight>
void UndirectedGraph<Weight>::addVertex(int id, const std::string &label)
{
    if (hasVertex(id))
        return;
    vertices[id] = Vertex(id, label);
    adj[id] = std::vector<std::pair<int, Weight>>();
}

template <typename Weight>
void UndirectedGraph<Weight>::removeVertex(int id)
{
    if (!hasVertex(id))
        return;

    // Удаляем все ребра, связанные с этой вершиной
    for (auto &[neighbor, weight] : adj[id])
    {
        auto &neighborList = adj[neighbor];
        neighborList.erase(
            std::remove_if(neighborList.begin(), neighborList.end(),
                           [id](const std::pair<int, Weight> &p)
                           { return p.first == id; }),
            neighborList.end());
        edgeCount--;
    }

    vertices.erase(id);
    adj.erase(id);
}

template <typename Weight>
void UndirectedGraph<Weight>::addEdge(int from, int to, Weight weight)
{
    if (!hasVertex(from) || !hasVertex(to))
        return;
    if (from == to)
        return; // Нет петель

    // Проверяем, есть ли уже такое ребро
    for (auto &p : adj[from])
    {
        if (p.first == to)
        {
            p.second = weight; // Обновляем вес
            return;
        }
    }

    // Добавляем ребро (неориентированный граф - добавляем в обе стороны)
    adj[from].push_back({to, weight});
    adj[to].push_back({from, weight});
    edgeCount++;
}

template <typename Weight>
void UndirectedGraph<Weight>::removeEdge(int from, int to)
{
    if (!hasVertex(from) || !hasVertex(to))
        return;

    bool removed = false;

    // Удаляем из списка смежности
    auto &fromList = adj[from];
    auto it = std::find_if(fromList.begin(), fromList.end(),
                           [to](const std::pair<int, Weight> &p)
                           { return p.first == to; });
    if (it != fromList.end())
    {
        fromList.erase(it);
        removed = true;
    }

    auto &toList = adj[to];
    auto it2 = std::find_if(toList.begin(), toList.end(),
                            [from](const std::pair<int, Weight> &p)
                            { return p.first == from; });
    if (it2 != toList.end())
    {
        toList.erase(it2);
    }

    if (removed)
        edgeCount--;
}

template <typename Weight>
bool UndirectedGraph<Weight>::hasVertex(int id) const
{
    return vertices.find(id) != vertices.end();
}

template <typename Weight>
bool UndirectedGraph<Weight>::hasEdge(int from, int to) const
{
    if (!hasVertex(from) || !hasVertex(to))
        return false;

    const auto &list = adj.at(from);
    return std::any_of(list.begin(), list.end(),
                       [to](const std::pair<int, Weight> &p)
                       { return p.first == to; });
}

template <typename Weight>
const Vertex &UndirectedGraph<Weight>::getVertex(int id) const
{
    static Vertex empty;
    auto it = vertices.find(id);
    if (it == vertices.end())
        return empty;
    return it->second;
}

template <typename Weight>
Weight UndirectedGraph<Weight>::getEdgeWeight(int from, int to) const
{
    if (!hasVertex(from) || !hasVertex(to))
        return Weight();

    const auto &list = adj.at(from);
    for (const auto &p : list)
    {
        if (p.first == to)
            return p.second;
    }
    return Weight();
}

template <typename Weight>
std::vector<int> UndirectedGraph<Weight>::getAdjacentVertices(int id) const
{
    std::vector<int> result;
    if (!hasVertex(id))
        return result;

    const auto &list = adj.at(id);
    for (const auto &[neighbor, weight] : list)
    {
        result.push_back(neighbor);
    }
    return result;
}

template <typename Weight>
const std::vector<std::pair<int, Weight>> &UndirectedGraph<Weight>::getAdjacencyList(int id) const
{
    static const std::vector<std::pair<int, Weight>> empty;
    auto it = adj.find(id);
    if (it == adj.end())
        return empty;
    return it->second;
}

template <typename Weight>
std::vector<int> UndirectedGraph<Weight>::getAllVertices() const
{
    std::vector<int> result;
    for (const auto &[id, vertex] : vertices)
    {
        result.push_back(id);
    }
    return result;
}

template <typename Weight>
std::vector<typename UndirectedGraph<Weight>::EdgeType> UndirectedGraph<Weight>::getAllEdges() const
{
    std::vector<EdgeType> result;
    std::set<std::pair<int, int>> seen;

    for (const auto &[from, list] : adj)
    {
        for (const auto &[to, weight] : list)
        {
            int minId = std::min(from, to);
            int maxId = std::max(from, to);

            if (seen.find({minId, maxId}) == seen.end())
            {
                result.push_back({from, to, weight});
                seen.insert({minId, maxId});
            }
        }
    }

    return result;
}

template <typename Weight>
void UndirectedGraph<Weight>::clear()
{
    vertices.clear();
    adj.clear();
    edgeCount = 0;
}

template <typename Weight>
void UndirectedGraph<Weight>::printInfo() const
{
    std::cout << "=== Graph Info ===" << std::endl;
    std::cout << "Vertices: " << vertices.size() << std::endl;
    std::cout << "Edges: " << edgeCount << std::endl;
    std::cout << "Adjacency List:" << std::endl;

    for (const auto &[id, list] : adj)
    {
        std::cout << "  " << id << " (" << vertices.at(id).label << "): ";
        for (const auto &[neighbor, weight] : list)
        {
            std::cout << neighbor << "(" << weight << ") ";
        }
        std::cout << std::endl;
    }
}

#endif // GRAPH_H