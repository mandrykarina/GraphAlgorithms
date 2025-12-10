"""
graph_visualization.py
Визуализация графа с помощью matplotlib и networkx
"""

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import FancyBboxPatch, FancyArrowPatch
import networkx as nx
import numpy as np
from typing import Dict, List, Tuple, Optional, Callable

class GraphVisualizer:
    """Визуализация графов"""
    
    def __init__(self, figsize: Tuple[int, int] = (12, 10)):
        self.figsize = figsize
        self.G = None
        self.pos = None
        self.fig = None
        self.ax = None
    
    def create_from_dict(self, graph_data: Dict) -> None:
        """Создать NetworkX граф из dictionary"""
        self.G = nx.Graph()
        
        # Добавляем вершины
        for v in graph_data['vertices']:
            self.G.add_node(v)
        
        # Добавляем ребра с весами
        for edge in graph_data['edges']:
            if len(edge) == 3:
                v1, v2, weight = edge
                self.G.add_edge(v1, v2, weight=weight)
            else:
                v1, v2 = edge
                self.G.add_edge(v1, v2, weight=1.0)
    
    def compute_layout(self, layout: str = 'spring') -> None:
        """Вычислить позиции вершин"""
        if self.G is None:
            raise ValueError("Graph not created. Use create_from_dict() first")
        
        if layout == 'spring':
            self.pos = nx.spring_layout(self.G, k=2, iterations=50, seed=42)
        elif layout == 'circular':
            self.pos = nx.circular_layout(self.G)
        elif layout == 'kamada_kawai':
            self.pos = nx.kamada_kawai_layout(self.G)
        else:
            self.pos = nx.spring_layout(self.G, k=2, iterations=50, seed=42)
    
    def draw_base_graph(self, 
                       node_color: str = '#3498db',
                       node_size: int = 600,
                       edge_color: str = '#95a5a6',
                       edge_width: float = 1.5,
                       font_size: int = 10) -> None:
        """Нарисовать базовый граф"""
        if self.G is None or self.pos is None:
            raise ValueError("Graph or layout not initialized")
        
        self.fig, self.ax = plt.subplots(figsize=self.figsize)
        
        # Нарисовать ребра
        nx.draw_networkx_edges(
            self.G, self.pos,
            edge_color=edge_color,
            width=edge_width,
            ax=self.ax,
            alpha=0.6
        )
        
        # Нарисовать вершины
        nx.draw_networkx_nodes(
            self.G, self.pos,
            node_color=node_color,
            node_size=node_size,
            ax=self.ax,
            edgecolors='#2c3e50',
            linewidths=2.0
        )
        
        # Нарисовать метки вершин
        nx.draw_networkx_labels(
            self.G, self.pos,
            font_size=font_size,
            font_color='white',
            font_weight='bold',
            ax=self.ax
        )
        
        # Нарисовать вес рёбер
        edge_labels = {(u, v): f"{d['weight']:.1f}" 
                       for u, v, d in self.G.edges(data=True)}
        nx.draw_networkx_edge_labels(
            self.G, self.pos,
            edge_labels,
            font_size=8,
            ax=self.ax
        )
        
        self.ax.set_title("Original Graph", fontsize=14, fontweight='bold', pad=20)
        self.ax.axis('off')
    
    def highlight_path(self, path: List[int], 
                      node_color: Optional[str] = '#e74c3c',
                      edge_color: Optional[str] = '#e74c3c') -> None:
        """Подсветить путь на графе"""
        if not path or self.ax is None:
            return
        
        # Подсветить вершины пути
        path_nodes = path
        other_nodes = [n for n in self.G.nodes() if n not in path_nodes]
        
        nx.draw_networkx_nodes(
            self.G, self.pos,
            nodelist=path_nodes,
            node_color=node_color,
            node_size=700,
            ax=self.ax,
            edgecolors='#c0392b',
            linewidths=3.0
        )
        
        # Подсветить рёбра пути
        path_edges = [(path[i], path[i+1]) for i in range(len(path)-1)]
        nx.draw_networkx_edges(
            self.G, self.pos,
            edgelist=path_edges,
            edge_color=edge_color,
            width=3.5,
            ax=self.ax,
            alpha=0.9
        )
    
    def highlight_mst_edges(self, mst_edges: List[Tuple[int, int]],
                           edge_color: str = '#27ae60',
                           edge_width: float = 3.0) -> None:
        """Подсветить рёбра MST"""
        if not mst_edges or self.ax is None:
            return
        
        # Нормализуем рёбра (направление не важно)
        normalized_edges = [tuple(sorted([u, v])) for u, v, _ in mst_edges]
        
        nx.draw_networkx_edges(
            self.G, self.pos,
            edgelist=normalized_edges,
            edge_color=edge_color,
            width=edge_width,
            ax=self.ax,
            alpha=0.95
        )
    
    def highlight_colored_nodes(self, coloring: Dict[int, int],
                               color_map: Optional[Dict[int, str]] = None) -> None:
        """Подсветить раскрашенные вершины"""
        if not coloring or self.ax is None:
            return
        
        # Стандартный набор цветов, если не предоставлен
        if color_map is None:
            colors = ['#e74c3c', '#3498db', '#2ecc71', '#f39c12', '#9b59b6', '#1abc9c']
            color_map = {i: colors[i % len(colors)] for i in set(coloring.values())}
        
        for color_id, color in color_map.items():
            nodes = [v for v, c in coloring.items() if c == color_id]
            if nodes:
                nx.draw_networkx_nodes(
                    self.G, self.pos,
                    nodelist=nodes,
                    node_color=color,
                    node_size=700,
                    ax=self.ax,
                    edgecolors='#2c3e50',
                    linewidths=2.0
                )
    
    def highlight_components(self, components: List[List[int]],
                            color_map: Optional[Dict[int, str]] = None) -> None:
        """Подсветить компоненты связности"""
        if not components or self.ax is None:
            return
        
        if color_map is None:
            colors = ['#3498db', '#e74c3c', '#2ecc71', '#f39c12', '#9b59b6', '#1abc9c']
            color_map = {i: colors[i % len(colors)] for i in range(len(components))}
        
        for comp_id, component in enumerate(components):
            color = color_map.get(comp_id, '#95a5a6')
            nx.draw_networkx_nodes(
                self.G, self.pos,
                nodelist=component,
                node_color=color,
                node_size=700,
                ax=self.ax,
                edgecolors='#2c3e50',
                linewidths=2.0
            )
    
    def set_title(self, title: str) -> None:
        """Установить заголовок"""
        if self.ax is not None:
            self.ax.set_title(title, fontsize=14, fontweight='bold', pad=20)
    
    def show(self) -> None:
        """Показать график"""
        if self.fig is not None:
            plt.tight_layout()
            plt.show()
    
    def save(self, filename: str) -> None:
        """Сохранить график в файл"""
        if self.fig is not None:
            self.fig.savefig(filename, dpi=150, bbox_inches='tight')
            print(f"Graph saved to {filename}")


if __name__ == "__main__":
    from graph_generator import GraphGenerator
    
    # Тест
    gen = GraphGenerator()
    graph = gen.generate_random_graph(8, edge_probability=0.3, connected=True)
    
    viz = GraphVisualizer()
    viz.create_from_dict(graph)
    viz.compute_layout('spring')
    viz.draw_base_graph()
    viz.show()