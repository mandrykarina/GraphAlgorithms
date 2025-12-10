#include <iostream>
#include <cassert>
#include <iomanip>
#include "test_utils.h"
#include "graph/graph.h"

using Graph = UndirectedGraph<double>;

// ============================================================================
// UNIT TESTS ДЛЯ СТРУКТУРЫ ГРАФА
// ============================================================================

// ============================================================================
// ТЕСТЫ ВЕРШИН
// ============================================================================

void testAddVertex()
{
    std::cout << "\nTest: addVertex()" << std::endl;
    Graph g;

    g.addVertex(0, "A");
    assertEqual(g.hasVertex(0), "hasVertex(0) returns true");
    assertEqual(g.getVertexCount() == 1, "getVertexCount() == 1");
    assertEqual(g.getVertex(0).label == "A", "vertex label is 'A'");

    g.addVertex(1, "B");
    g.addVertex(2, "C");
    assertEqual(g.getVertexCount() == 3, "3 vertices added");
}

void testAddVertexDuplicate()
{
    std::cout << "\nTest: addVertex() duplicate" << std::endl;
    Graph g;

    g.addVertex(0, "A");
    g.addVertex(0, "B"); // Должен игнорировать
    assertEqual(g.getVertexCount() == 1, "duplicate vertex ignored");
    assertEqual(g.getVertex(0).label == "A", "original label preserved");
}

void testRemoveVertex()
{
    std::cout << "\nTest: removeVertex()" << std::endl;
    Graph g;

    g.addVertex(0, "A");
    g.addVertex(1, "B");
    g.addVertex(2, "C");

    g.removeVertex(1);
    assertEqual(g.getVertexCount() == 2, "vertex count after removal");
    assertEqual(!g.hasVertex(1), "removed vertex not found");
    assertEqual(g.hasVertex(0) && g.hasVertex(2), "other vertices remain");
}

void testRemoveNonexistent()
{
    std::cout << "\nTest: removeVertex() nonexistent" << std::endl;
    Graph g;

    g.addVertex(0, "A");
    g.removeVertex(1); // не существует

    assertEqual(g.getVertexCount() == 1, "graph unchanged");
}

// ============================================================================
// ТЕСТЫ РЕБЕР
// ============================================================================

void testAddEdge()
{
    std::cout << "\nTest: addEdge()" << std::endl;
    Graph g;

    g.addVertex(0);
    g.addVertex(1);
    g.addEdge(0, 1, 5.0);

    assertEqual(g.hasEdge(0, 1), "hasEdge(0, 1)");
    assertEqual(g.hasEdge(1, 0), "hasEdge(1, 0) - undirected");
    assertEqual(g.getEdgeWeight(0, 1) == 5.0, "weight is 5.0");
    assertEqual(g.getEdgeCount() == 1, "edge count is 1");
}

void testAddEdgeMultiple()
{
    std::cout << "\nTest: addEdge() multiple" << std::endl;
    Graph g;

    for (int i = 0; i < 5; i++)
    {
        g.addVertex(i);
    }

    g.addEdge(0, 1, 1.0);
    g.addEdge(0, 2, 2.0);
    g.addEdge(1, 2, 3.0);
    g.addEdge(2, 3, 4.0);

    assertEqual(g.getEdgeCount() == 4, "4 edges added");
}

void testAddEdgeUpdate()
{
    std::cout << "\nTest: addEdge() update weight" << std::endl;
    Graph g;

    g.addVertex(0);
    g.addVertex(1);
    g.addEdge(0, 1, 5.0);
    g.addEdge(0, 1, 10.0); // обновляем вес

    assertEqual(g.getEdgeCount() == 1, "still 1 edge");
    assertEqual(g.getEdgeWeight(0, 1) == 10.0, "weight updated to 10.0");
}

void testRemoveEdge()
{
    std::cout << "\nTest: removeEdge()" << std::endl;
    Graph g;

    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, 1, 1.0);
    g.addEdge(1, 2, 2.0);

    g.removeEdge(0, 1);

    assertEqual(!g.hasEdge(0, 1), "edge removed");
    assertEqual(!g.hasEdge(1, 0), "undirected edge removed both ways");
    assertEqual(g.getEdgeCount() == 1, "1 edge remains");
    assertEqual(g.hasEdge(1, 2), "other edge remains");
}

void testAddEdgeNoVertices()
{
    std::cout << "\nTest: addEdge() no vertices" << std::endl;
    Graph g;

    g.addEdge(0, 1, 1.0); // вершин нет
    assertEqual(g.getEdgeCount() == 0, "edge not added");
}

// ============================================================================
// ТЕСТЫ СОСЕДЕЙ И СПИСКОВ СМЕЖНОСТИ
// ============================================================================

void testGetAdjacentVertices()
{
    std::cout << "\nTest: getAdjacentVertices()" << std::endl;
    Graph g;

    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addVertex(3);
    g.addEdge(0, 1, 1.0);
    g.addEdge(0, 2, 2.0);
    g.addEdge(0, 3, 3.0);

    auto neighbors = g.getAdjacentVertices(0);
    assertEqual(neighbors.size() == 3, "3 neighbors");

    // Проверяем содержит ли 1, 2, 3
    bool has1 = std::find(neighbors.begin(), neighbors.end(), 1) != neighbors.end();
    bool has2 = std::find(neighbors.begin(), neighbors.end(), 2) != neighbors.end();
    bool has3 = std::find(neighbors.begin(), neighbors.end(), 3) != neighbors.end();

    assertEqual(has1 && has2 && has3, "contains correct neighbors");
}

void testGetAdjacencyList()
{
    std::cout << "\nTest: getAdjacencyList()" << std::endl;
    Graph g;

    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, 1, 5.0);
    g.addEdge(0, 2, 10.0);

    auto list = g.getAdjacencyList(0);
    assertEqual(list.size() == 2, "2 edges in adjacency list");
}

// ============================================================================
// ТЕСТЫ ГРАНИЧНЫХ СЛУЧАЕВ
// ============================================================================

void testEmptyGraph()
{
    std::cout << "\nTest: empty graph" << std::endl;
    Graph g;

    assertEqual(g.getVertexCount() == 0, "0 vertices");
    assertEqual(g.getEdgeCount() == 0, "0 edges");
    assertEqual(g.getAllVertices().size() == 0, "empty vertex list");
    assertEqual(g.getAllEdges().size() == 0, "empty edge list");
}

void testSingleVertex()
{
    std::cout << "\nTest: single vertex" << std::endl;
    Graph g;

    g.addVertex(0);
    assertEqual(g.getVertexCount() == 1, "1 vertex");
    assertEqual(g.getEdgeCount() == 0, "0 edges");
}

void testNoLoops()
{
    std::cout << "\nTest: no self-loops" << std::endl;
    Graph g;

    g.addVertex(0);
    g.addEdge(0, 0, 5.0); // попытка петли

    assertEqual(g.getEdgeCount() == 0, "self-loop not added");
}

void testDisconnectedGraph()
{
    std::cout << "\nTest: disconnected components" << std::endl;
    Graph g;

    // Компонента 1: 0-1
    g.addVertex(0);
    g.addVertex(1);
    g.addEdge(0, 1, 1.0);

    // Компонента 2: 2-3
    g.addVertex(2);
    g.addVertex(3);
    g.addEdge(2, 3, 2.0);

    // Изолированная вершина
    g.addVertex(4);

    assertEqual(g.getVertexCount() == 5, "5 vertices");
    assertEqual(g.getEdgeCount() == 2, "2 edges");

    auto adj0 = g.getAdjacentVertices(0);
    auto adj4 = g.getAdjacentVertices(4);

    assertEqual(adj0.size() == 1, "vertex 0 has 1 neighbor");
    assertEqual(adj4.size() == 0, "vertex 4 isolated");
}

// ============================================================================
// ТЕСТЫ ПОЛУЧЕНИЯ ВСЕХ ЭЛЕМЕНТОВ
// ============================================================================

void testGetAllVertices()
{
    std::cout << "\nTest: getAllVertices()" << std::endl;
    Graph g;

    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);

    auto all = g.getAllVertices();
    assertEqual(all.size() == 3, "3 vertices");
}

void testGetAllEdges()
{
    std::cout << "\nTest: getAllEdges()" << std::endl;
    Graph g;

    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, 1, 1.0);
    g.addEdge(1, 2, 2.0);

    auto all = g.getAllEdges();
    assertEqual(all.size() == 2, "2 edges (not 4 with undirected)");
}

// ============================================================================
// ТЕСТЫ УДАЛЕНИЯ ВЕРШИНЫ С РЕБРАМИ
// ============================================================================

void testRemoveVertexWithEdges()
{
    std::cout << "\nTest: removeVertex() removes edges" << std::endl;
    Graph g;

    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, 1, 1.0);
    g.addEdge(1, 2, 2.0);
    g.addEdge(0, 2, 3.0);

    g.removeVertex(1); // удаляем середину

    assertEqual(g.getVertexCount() == 2, "2 vertices remain");
    assertEqual(g.getEdgeCount() == 1, "1 edge remains (0-2)");
    assertEqual(g.hasEdge(0, 1) == false, "edge 0-1 removed");
    assertEqual(g.hasEdge(1, 2) == false, "edge 1-2 removed");
    assertEqual(g.hasEdge(0, 2) == true, "edge 0-2 remains");
}

void testRemoveVertexAllEdges()
{
    std::cout << "\nTest: removeVertex() with multiple edges" << std::endl;
    Graph g;

    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addVertex(3);

    g.addEdge(0, 1, 1.0);
    g.addEdge(0, 2, 2.0);
    g.addEdge(0, 3, 3.0);

    g.removeVertex(0);

    assertEqual(g.getVertexCount() == 3, "3 vertices remain");
    assertEqual(g.getEdgeCount() == 0, "all edges removed");
}

// ============================================================================
// ГЛАВНАЯ ФУНКЦИЯ ЗАПУСКА ВСЕХ ТЕСТОВ
// ============================================================================

int runall()
{
    std::cout << "\n"
              << std::string(70, '=') << std::endl;
    std::cout << "GRAPH UNIT TESTS" << std::endl;
    std::cout << std::string(70, '=') << std::endl;

    passedTests = 0;
    failedTests = 0;

    // Тесты вершин
    testAddVertex();
    testAddVertexDuplicate();
    testRemoveVertex();
    testRemoveNonexistent();

    // Тесты ребер
    testAddEdge();
    testAddEdgeMultiple();
    testAddEdgeUpdate();
    testRemoveEdge();
    testAddEdgeNoVertices();

    // Соседи
    testGetAdjacentVertices();
    testGetAdjacencyList();

    // Граничные случаи
    testEmptyGraph();
    testSingleVertex();
    testNoLoops();
    testDisconnectedGraph();

    // Получение всех
    testGetAllVertices();
    testGetAllEdges();

    // Удаление с ребрами
    testRemoveVertexWithEdges();
    testRemoveVertexAllEdges();

    // Итоговый результат
    std::cout << "\n"
              << std::string(70, '=') << std::endl;
    std::cout << "RESULTS: " << passedTests << " passed, " << failedTests << " failed" << std::endl;
    std::cout << std::string(70, '=') << std::endl;

    return failedTests == 0 ? 0 : 1;
}
int main()
{
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║        GRAPH UNIT TESTS - STANDALONE VERSION             ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;

    passedTests = 0;
    failedTests = 0;

    // Тесты вершин
    testAddVertex();
    testAddVertexDuplicate();
    testRemoveVertex();
    testRemoveNonexistent();

    // Тесты ребер
    testAddEdge();
    testAddEdgeMultiple();
    testAddEdgeUpdate();
    testRemoveEdge();
    testAddEdgeNoVertices();

    // Соседи
    testGetAdjacentVertices();
    testGetAdjacencyList();

    // Граничные случаи
    testEmptyGraph();
    testSingleVertex();
    testNoLoops();
    testDisconnectedGraph();

    // Получение всех
    testGetAllVertices();
    testGetAllEdges();

    // Удаление с ребрами
    testRemoveVertexWithEdges();
    testRemoveVertexAllEdges();

    // Результаты
    std::cout << "\n"
              << std::string(60, '=') << std::endl;
    std::cout << "FINAL RESULTS: " << passedTests << " PASSED, "
              << failedTests << " FAILED" << std::endl;
    std::cout << std::string(60, '=') << std::endl
              << std::endl;

    return failedTests == 0 ? 0 : 1;
}