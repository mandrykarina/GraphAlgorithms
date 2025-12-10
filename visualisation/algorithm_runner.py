"""
algorithm_runner.py
Запуск алгоритмов через subprocess и обработка результатов
"""

import subprocess
import json
import os
import tempfile
import sys
from typing import Dict, List, Tuple, Any

class AlgorithmRunner:
    """Запускает C++ алгоритмы и возвращает результаты"""
    
    def __init__(self, cpp_executable: str = "main.exe"):
        self.cpp_exe = cpp_executable
        self.temp_graph_file = None
        self.temp_output_file = None
    
    @staticmethod
    def graph_to_cpp_format(graph_data: Dict) -> str:
        """
        Конвертирует граф в формат для C++ программы
        Формат: количество вершин, количество ребер, потом все ребра (from to weight)
        """
        vertices = graph_data['vertices']
        edges = graph_data['edges']
        
        lines = []
        lines.append(f"{len(vertices)} {len(edges)}")
        
        for edge in edges:
            if len(edge) == 3:
                v1, v2, weight = edge
            else:
                v1, v2 = edge
                weight = 1.0
            lines.append(f"{v1} {v2} {weight:.2f}")
        
        return "\n".join(lines)
    
    @staticmethod
    def save_graph_temp(graph_data: Dict) -> str:
        """Сохранить граф во временный файл"""
        content = AlgorithmRunner.graph_to_cpp_format(graph_data)
        with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False) as f:
            f.write(content)
            return f.name
    
    @staticmethod
    def run_algorithm(algorithm_name: str, graph_data: Dict) -> Dict[str, Any]:
        """
        Запустить алгоритм
        
        Локально реализуем базовые алгоритмы на Python
        для демонстрации
        """
        
        if algorithm_name == "dijkstra":
            return AlgorithmRunner.run_dijkstra(graph_data)
        elif algorithm_name == "tsp":
            return AlgorithmRunner.run_tsp(graph_data)
        elif algorithm_name == "mst":
            return AlgorithmRunner.run_mst(graph_data)
        elif algorithm_name == "connectivity":
            return AlgorithmRunner.run_connectivity(graph_data)
        elif algorithm_name == "coloring":
            return AlgorithmRunner.run_coloring(graph_data)
        elif algorithm_name == "hotel":
            return AlgorithmRunner.run_hotel_optimization(graph_data)
        else:
            return {"error": f"Unknown algorithm: {algorithm_name}"}
    
    @staticmethod
    def run_dijkstra(graph_data: Dict, start: int = 0) -> Dict[str, Any]:
        """Реализация Dijkstra на Python"""
        import heapq
        
        vertices = graph_data['vertices']
        if not vertices or start not in vertices:
            start = vertices[0] if vertices else 0
        
        # Построить граф смежности
        adj = {v: [] for v in vertices}
        for edge in graph_data['edges']:
            if len(edge) == 3:
                v1, v2, weight = edge
            else:
                v1, v2 = edge
                weight = 1.0
            adj[v1].append((v2, weight))
            adj[v2].append((v1, weight))
        
        # Dijkstra
        distances = {v: float('inf') for v in vertices}
        distances[start] = 0
        parent = {v: -1 for v in vertices}
        pq = [(0, start)]
        visited = set()
        
        while pq:
            curr_dist, u = heapq.heappop(pq)
            
            if u in visited:
                continue
            visited.add(u)
            
            for v, weight in adj[u]:
                if v not in visited and distances[u] + weight < distances[v]:
                    distances[v] = distances[u] + weight
                    parent[v] = u
                    heapq.heappush(pq, (distances[v], v))
        
        return {
            "algorithm": "Dijkstra",
            "distances": {str(v): float(distances[v]) for v in vertices},
            "parent": {str(v): parent[v] for v in vertices},
            "start": start,
            "success": True
        }
    
    @staticmethod
    def run_tsp(graph_data: Dict, start: int = 0) -> Dict[str, Any]:
        """Приближенный TSP - Nearest Neighbor"""
        vertices = graph_data['vertices']
        if not vertices:
            return {"error": "Empty graph", "success": False}
        
        start = vertices[0]
        
        # Построить матрицу весов
        adj = {v: {u: float('inf') for u in vertices} for v in vertices}
        for edge in graph_data['edges']:
            if len(edge) == 3:
                v1, v2, weight = edge
            else:
                v1, v2 = edge
                weight = 1.0
            adj[v1][v2] = weight
            adj[v2][v1] = weight
        
        # Nearest Neighbor
        tour = [start]
        visited = {start}
        current = start
        total_distance = 0.0
        
        while len(visited) < len(vertices):
            nearest = -1
            min_dist = float('inf')
            
            for v in vertices:
                if v not in visited and adj[current][v] < min_dist:
                    min_dist = adj[current][v]
                    nearest = v
            
            if nearest == -1:
                break
            
            tour.append(nearest)
            visited.add(nearest)
            total_distance += min_dist
            current = nearest
        
        # Вернуться к начальной вершине
        if len(tour) == len(vertices):
            total_distance += adj[current][start]
            tour.append(start)
        
        return {
            "algorithm": "TSP (Nearest Neighbor)",
            "tour": tour,
            "total_distance": round(total_distance, 2),
            "vertices_visited": len(tour) - 1,
            "success": True
        }
    
    @staticmethod
    def run_mst(graph_data: Dict) -> Dict[str, Any]:
        """MST - Kruskal Algorithm"""
        vertices = graph_data['vertices']
        edges = graph_data['edges']
        
        if not vertices:
            return {"error": "Empty graph", "success": False}
        
        # Union-Find
        parent = {v: v for v in vertices}
        rank = {v: 0 for v in vertices}
        
        def find(x):
            if parent[x] != x:
                parent[x] = find(parent[x])
            return parent[x]
        
        def union(x, y):
            px, py = find(x), find(y)
            if px == py:
                return False
            if rank[px] < rank[py]:
                px, py = py, px
            parent[py] = px
            if rank[px] == rank[py]:
                rank[px] += 1
            return True
        
        # Сортируем ребра
        sorted_edges = []
        for edge in edges:
            if len(edge) == 3:
                v1, v2, weight = edge
            else:
                v1, v2 = edge
                weight = 1.0
            sorted_edges.append((weight, v1, v2))
        sorted_edges.sort()
        
        # Kruskal
        mst_edges = []
        total_weight = 0.0
        
        for weight, u, v in sorted_edges:
            if union(u, v):
                mst_edges.append((u, v, weight))
                total_weight += weight
        
        return {
            "algorithm": "MST (Kruskal)",
            "mst_edges": mst_edges,
            "total_weight": round(total_weight, 2),
            "num_edges": len(mst_edges),
            "is_connected": len(mst_edges) == len(vertices) - 1,
            "success": True
        }
    
    @staticmethod
    def run_connectivity(graph_data: Dict) -> Dict[str, Any]:
        """Компоненты связности - DFS"""
        vertices = graph_data['vertices']
        edges = graph_data['edges']
        
        if not vertices:
            return {"error": "Empty graph", "success": False}
        
        # Построить граф смежности
        adj = {v: [] for v in vertices}
        for edge in edges:
            v1, v2 = edge[0], edge[1]
            adj[v1].append(v2)
            adj[v2].append(v1)
        
        # DFS
        visited = set()
        components = []
        
        def dfs(v, component):
            visited.add(v)
            component.append(v)
            for u in adj[v]:
                if u not in visited:
                    dfs(u, component)
        
        for v in vertices:
            if v not in visited:
                component = []
                dfs(v, component)
                components.append(sorted(component))
        
        return {
            "algorithm": "Connectivity (DFS)",
            "components": components,
            "num_components": len(components),
            "is_connected": len(components) == 1,
            "largest_component_size": max(len(c) for c in components) if components else 0,
            "success": True
        }
    
    @staticmethod
    def run_coloring(graph_data: Dict) -> Dict[str, Any]:
        """Раскраска графа - Greedy"""
        vertices = graph_data['vertices']
        edges = graph_data['edges']
        
        if not vertices:
            return {"error": "Empty graph", "success": False}
        
        # Построить граф смежности
        adj = {v: set() for v in vertices}
        for edge in edges:
            v1, v2 = edge[0], edge[1]
            adj[v1].add(v2)
            adj[v2].add(v1)
        
        # Greedy coloring
        coloring = {v: -1 for v in vertices}
        
        for v in vertices:
            # Найти используемые цвета у соседей
            neighbor_colors = {coloring[u] for u in adj[v] if coloring[u] != -1}
            
            # Найти первый доступный цвет
            color = 0
            while color in neighbor_colors:
                color += 1
            
            coloring[v] = color
        
        chromatic_number = max(coloring.values()) + 1 if coloring else 0
        
        return {
            "algorithm": "Graph Coloring (Greedy)",
            "coloring": coloring,
            "chromatic_number": chromatic_number,
            "colors_used": list(range(chromatic_number)),
            "success": True
        }
    
    @staticmethod
    def run_hotel_optimization(graph_data: Dict) -> Dict[str, Any]:
        """Hotel Optimization - K-Centers approximation"""
        vertices = graph_data['vertices']
        edges = graph_data['edges']
        
        if not vertices:
            return {"error": "Empty graph", "success": False}
        
        # Построить матрицу расстояний (BFS)
        from collections import deque
        
        adj = {v: [] for v in vertices}
        for edge in edges:
            v1, v2 = edge[0], edge[1]
            adj[v1].append(v2)
            adj[v2].append(v1)
        
        def bfs_distance(start, end):
            if start == end:
                return 0
            visited = {start}
            q = deque([(start, 0)])
            while q:
                u, dist = q.popleft()
                for v in adj[u]:
                    if v == end:
                        return dist + 1
                    if v not in visited:
                        visited.add(v)
                        q.append((v, dist + 1))
            return float('inf')
        
        # Выбираем центры грeedily (K-Centers)
        k = max(1, len(vertices) // 3 + 1)  # Примерно 1/3 вершин
        centers = [vertices[0]]
        
        for _ in range(min(k - 1, len(vertices) - 1)):
            farthest = -1
            max_min_dist = -1
            
            for v in vertices:
                if v in centers:
                    continue
                min_dist = min(bfs_distance(v, c) for c in centers)
                if min_dist > max_min_dist:
                    max_min_dist = min_dist
                    farthest = v
            
            if farthest != -1:
                centers.append(farthest)
        
        # Назначаем вершины центрам
        assignment = {}
        max_dist = 0
        for v in vertices:
            nearest = min(centers, key=lambda c: bfs_distance(v, c))
            dist = bfs_distance(v, nearest)
            assignment[v] = nearest
            max_dist = max(max_dist, dist)
        
        return {
            "algorithm": "Hotel Optimization (K-Centers)",
            "centers": centers,
            "num_centers": len(centers),
            "assignment": assignment,
            "max_distance": max_dist,
            "success": True
        }


if __name__ == "__main__":
    from graph_generator import GraphGenerator
    
    gen = GraphGenerator()
    graph = gen.generate_random_graph(6, edge_probability=0.4, connected=True)
    
    print("=== Testing Algorithms ===\n")
    
    for algo in ["dijkstra", "tsp", "mst", "connectivity", "coloring", "hotel"]:
        result = AlgorithmRunner.run_algorithm(algo, graph)
        print(f"{result['algorithm']}:")
        print(json.dumps(result, indent=2, default=str))
        print()