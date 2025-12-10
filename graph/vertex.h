#ifndef VERTEX_H
#define VERTEX_H

#include <string>

/**
 * Структура вершины графа
 */
struct Vertex
{
    int id;            // Уникальный идентификатор
    std::string label; // Метка вершины
    double x, y;       // Координаты для визуализации
    int color;         // Цвет при раскраске (-1 если не раскрашена)

    Vertex() : id(-1), label(""), x(0), y(0), color(-1) {}
    Vertex(int id, const std::string &label = "")
        : id(id), label(label), x(0), y(0), color(-1) {}
};

#endif // VERTEX_H