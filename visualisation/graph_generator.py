"""
graph_generator.py
Генератор случайных неориентированных графов
"""

import random
import json
from typing import Dict, List, Tuple

class GraphGenerator:
    """Генератор случайных графов"""
    
    def __init__(self, seed: int = None):
        if seed is not None:
            random.seed(seed)
    
    @staticmethod
    def generate_random_graph(
        num_vertices: int,
        edge_probability: float = 0.3,
        min_weight: float = 1.0,
        max_weight: float = 10.0,
        connected: bool = True
    ) -> Dict:
        """
        Генерирует случайный неориентированный граф
        
        Args:
            num_vertices: количество вершин
            edge_probability: вероятность добавления ребра (0-1)
            min_weight: минимальный вес ребра
            max_weight: максимальный вес ребра
            connected: гарантировать связность графа
        
        Returns:
            Dictionary с вершинами и ребрами
        """
        if num_vertices < 1:
            raise ValueError("Количество вершин должно быть >= 1")
        
        if edge_probability < 0 or edge_probability > 1:
            raise ValueError("Вероятность должна быть 0-1")
        
        # Создаём вершины
        vertices = list(range(num_vertices))
        edges = set()
        
        # Если нужно гарантировать связность - создаём путь через все вершины
        if connected and num_vertices > 1:
            shuffled = vertices.copy()
            random.shuffle(shuffled)
            for i in range(len(shuffled) - 1):
                weight = round(random.uniform(min_weight, max_weight), 2)
                edge = tuple(sorted([shuffled[i], shuffled[i + 1]]))
                edges.add((edge[0], edge[1], weight))
        
        # Добавляем случайные ребра
        for v1 in range(num_vertices):
            for v2 in range(v1 + 1, num_vertices):
                if random.random() < edge_probability:
                    if (v1, v2) not in edges and (v2, v1) not in edges:
                        weight = round(random.uniform(min_weight, max_weight), 2)
                        edges.add((v1, v2, weight))
        
        return {
            'vertices': vertices,
            'edges': list(edges),
            'num_vertices': num_vertices,
            'num_edges': len(edges)
        }
    
    @staticmethod
    def generate_path_graph(num_vertices: int, edge_weight: float = 1.0) -> Dict:
        """Граф-путь (цепь)"""
        vertices = list(range(num_vertices))
        edges = []
        for i in range(num_vertices - 1):
            edges.append((i, i + 1, edge_weight))
        
        return {
            'vertices': vertices,
            'edges': edges,
            'num_vertices': num_vertices,
            'num_edges': len(edges)
        }
    
    @staticmethod
    def generate_cycle_graph(num_vertices: int, edge_weight: float = 1.0) -> Dict:
        """Граф-цикл"""
        vertices = list(range(num_vertices))
        edges = []
        for i in range(num_vertices):
            next_v = (i + 1) % num_vertices
            edges.append((i, next_v, edge_weight))
        
        return {
            'vertices': vertices,
            'edges': edges,
            'num_vertices': num_vertices,
            'num_edges': len(edges)
        }
    
    @staticmethod
    def generate_complete_graph(num_vertices: int, edge_weight: float = 1.0) -> Dict:
        """Полный граф (все с каждым)"""
        vertices = list(range(num_vertices))
        edges = []
        for v1 in range(num_vertices):
            for v2 in range(v1 + 1, num_vertices):
                edges.append((v1, v2, edge_weight))
        
        return {
            'vertices': vertices,
            'edges': edges,
            'num_vertices': num_vertices,
            'num_edges': len(edges)
        }
    
    @staticmethod
    def generate_grid_graph(rows: int, cols: int, edge_weight: float = 1.0) -> Dict:
        """Граф-сетка (grid)"""
        vertices = list(range(rows * cols))
        edges = []
        
        for r in range(rows):
            for c in range(cols):
                v = r * cols + c
                # Ребро вправо
                if c < cols - 1:
                    edges.append((v, v + 1, edge_weight))
                # Ребро вниз
                if r < rows - 1:
                    edges.append((v, v + cols, edge_weight))
        
        return {
            'vertices': vertices,
            'edges': edges,
            'num_vertices': len(vertices),
            'num_edges': len(edges)
        }
    
    @staticmethod
    def save_graph(graph: Dict, filename: str) -> None:
        """Сохранить граф в JSON"""
        with open(filename, 'w') as f:
            json.dump(graph, f, indent=2)
    
    @staticmethod
    def load_graph(filename: str) -> Dict:
        """Загрузить граф из JSON"""
        with open(filename, 'r') as f:
            return json.load(f)


if __name__ == "__main__":
    # Тест
    gen = GraphGenerator()
    g = gen.generate_random_graph(10, edge_probability=0.3, connected=True)
    print(f"Граф: {g['num_vertices']} вершин, {g['num_edges']} ребер")
    print(f"Ребра: {g['edges'][:5]}...")