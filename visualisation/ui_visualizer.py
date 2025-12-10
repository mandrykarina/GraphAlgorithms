"""
ui_visualizer.py - С ВЕСАМИ НА РЁБРАХ И ИСПРАВЛЕННЫМ CONNECTIVITY
ЗАПУСКАТЬ: python ui_visualizer.py
"""

import matplotlib.pyplot as plt
from matplotlib.widgets import Button, Slider, RadioButtons
import matplotlib.gridspec as gridspec
import numpy as np
import networkx as nx

from graph_generator import GraphGenerator
from algorithm_runner import AlgorithmRunner


class GraphVisualizerApp:
    """Главное приложение визуализации"""
    
    def __init__(self):
        self.graph_data = None
        self.current_results = {}
        self.selected_algorithm = None
        self.G = None
        self.pos = None
        
        # Окно
        self.fig = plt.figure(figsize=(20, 14))
        self.fig.suptitle('GRAPH ALGORITHMS VISUALIZATION', fontsize=18, fontweight='bold')
        
        # Сетка
        gs = gridspec.GridSpec(3, 3, figure=self.fig, hspace=0.35, wspace=0.3)
        
        # Граф в центре (большой)
        self.ax_main = self.fig.add_subplot(gs[0:2, 0:2])
        self.ax_main.set_facecolor('#f0f0f0')
        
        # Информация справа
        self.ax_info = self.fig.add_subplot(gs[0:2, 2])
        self.ax_info.axis('off')
        
        # ===== КНОПКИ =====
        # Первый ряд кнопок
        self.ax_btn_gen = self.fig.add_axes([0.05, 0.38, 0.08, 0.05])
        self.btn_gen = Button(self.ax_btn_gen, 'Generate', color='#3498db', hovercolor='#2980b9')
        self.btn_gen.on_clicked(self.on_generate_graph)
        
        self.ax_btn_dijkstra = self.fig.add_axes([0.14, 0.38, 0.08, 0.05])
        self.btn_dijkstra = Button(self.ax_btn_dijkstra, 'Dijkstra', color='#e74c3c', hovercolor='#c0392b')
        self.btn_dijkstra.on_clicked(lambda e: self.on_run_algorithm('dijkstra'))
        
        self.ax_btn_tsp = self.fig.add_axes([0.23, 0.38, 0.08, 0.05])
        self.btn_tsp = Button(self.ax_btn_tsp, 'TSP', color='#f39c12', hovercolor='#e67e22')
        self.btn_tsp.on_clicked(lambda e: self.on_run_algorithm('tsp'))
        
        self.ax_btn_mst = self.fig.add_axes([0.32, 0.38, 0.08, 0.05])
        self.btn_mst = Button(self.ax_btn_mst, 'MST', color='#2ecc71', hovercolor='#27ae60')
        self.btn_mst.on_clicked(lambda e: self.on_run_algorithm('mst'))
        
        # Второй ряд кнопок
        self.ax_btn_conn = self.fig.add_axes([0.05, 0.30, 0.08, 0.05])
        self.btn_conn = Button(self.ax_btn_conn, 'Connect', color='#9b59b6', hovercolor='#8e44ad')
        self.btn_conn.on_clicked(lambda e: self.on_run_algorithm('connectivity'))
        
        self.ax_btn_color = self.fig.add_axes([0.14, 0.30, 0.08, 0.05])
        self.btn_color = Button(self.ax_btn_color, 'Coloring', color='#1abc9c', hovercolor='#16a085')
        self.btn_color.on_clicked(lambda e: self.on_run_algorithm('coloring'))
        
        self.ax_btn_hotel = self.fig.add_axes([0.23, 0.30, 0.08, 0.05])
        self.btn_hotel = Button(self.ax_btn_hotel, 'Hotel', color='#34495e', hovercolor='#2c3e50')
        self.btn_hotel.on_clicked(lambda e: self.on_run_algorithm('hotel'))
        
        # ===== СЛАЙДЕРЫ =====
        self.ax_slider_v = self.fig.add_axes([0.05, 0.23, 0.35, 0.03])
        self.slider_vertices = Slider(self.ax_slider_v, 'Vertices', 3, 15, valinit=8, valstep=1, color='#3498db')
        
        self.ax_slider_p = self.fig.add_axes([0.05, 0.18, 0.35, 0.03])
        self.slider_prob = Slider(self.ax_slider_p, 'Prob', 0.1, 0.8, valinit=0.3, valstep=0.05, color='#e74c3c')
        
        # ===== RADIO BUTTONS =====
        self.ax_radio = self.fig.add_axes([0.05, 0.01, 0.35, 0.15])
        self.radio_graph_type = RadioButtons(
            self.ax_radio,
            ('Random', 'Circle', 'Grid', 'Complete', 'Path'),
            active=0
        )
        
        # Генерируем начальный граф
        self.on_generate_graph(None)
        
        # Инструкции
        print("\n" + "="*70)
        print("  GRAPH VISUALIZATION READY!".center(70))
        print("="*70)
        print("\n6 ALGORITHMS:")
        print("  1. Dijkstra    - shortest path (red)")
        print("  2. TSP         - tour (orange)")
        print("  3. MST         - tree (green)")
        print("  4. Connect     - components (colors)")
        print("  5. Coloring    - vertex coloring")
        print("  6. Hotel       - centers (red)")
        print("\nGRAPH TYPES: Random, Circle, Grid, Complete, Path")
        print("="*70 + "\n")
    
    def generate_graph_by_type(self):
        """Генерирует граф"""
        graph_type = self.radio_graph_type.value_selected
        num_vertices = int(self.slider_vertices.val)
        edge_prob = self.slider_prob.val
        
        gen = GraphGenerator()
        
        try:
            if graph_type == 'Random':
                self.graph_data = gen.generate_random_graph(num_vertices, edge_probability=edge_prob, connected=True)
            elif graph_type == 'Circle':
                self.graph_data = gen.generate_cycle_graph(num_vertices)
            elif graph_type == 'Grid':
                grid_size = max(2, int(np.sqrt(num_vertices)))
                self.graph_data = gen.generate_grid_graph(grid_size, grid_size)
            elif graph_type == 'Complete':
                self.graph_data = gen.generate_complete_graph(num_vertices)
            elif graph_type == 'Path':
                self.graph_data = gen.generate_path_graph(num_vertices)
            
            self.current_results = {}
            self.selected_algorithm = None
            
            # Создаём NetworkX граф
            self.create_networkx_graph()
            
            print(f"OK: {self.graph_data['num_vertices']} v, {self.graph_data['num_edges']} e")
        except Exception as e:
            print(f"ERROR: {e}")
    
    def create_networkx_graph(self):
        """Создаёт NetworkX граф из data"""
        self.G = nx.Graph()
        
        # Добавляем вершины
        for v in self.graph_data['vertices']:
            self.G.add_node(v)
        
        # Добавляем рёбра с весами
        for edge in self.graph_data['edges']:
            if isinstance(edge, dict):
                u, v, w = edge['source'], edge['target'], edge.get('weight', 1)
            else:
                u, v, w = edge[0], edge[1], edge[2] if len(edge) > 2 else 1
            self.G.add_edge(u, v, weight=w)
        
        # Вычисляем позиции
        self.pos = nx.spring_layout(self.G, k=0.5, iterations=50, seed=42)
    
    def on_generate_graph(self, event):
        """Нажата кнопка Generate"""
        self.generate_graph_by_type()
        self.draw_graph()
        self.update_info()
    
    def on_run_algorithm(self, algorithm_name):
        """Нажата кнопка алгоритма"""
        if self.graph_data is None:
            print("ERROR: Generate graph first!")
            return
        
        print(f"Running {algorithm_name}...")
        
        try:
            results = AlgorithmRunner.run_algorithm(algorithm_name, self.graph_data)
            self.current_results = results
            self.selected_algorithm = algorithm_name
            print(f"OK: {algorithm_name} done!")
            
            self.draw_graph()
            self.update_info()
        except Exception as e:
            print(f"ERROR: {e}")
    
    def draw_graph(self):
        """Рисует граф"""
        self.ax_main.clear()
        
        if self.G is None or self.pos is None:
            return
        
        # Базовый граф - серые рёбра
        nx.draw_networkx_edges(self.G, self.pos, ax=self.ax_main, width=1.5, alpha=0.6, edge_color='#888888')
        
        # НОВОЕ: Отображение весов на рёбрах
        edge_labels = nx.get_edge_attributes(self.G, 'weight')
        edge_labels = {k: f"{v:.1f}" for k, v in edge_labels.items()}
        nx.draw_networkx_edge_labels(self.G, self.pos, edge_labels, ax=self.ax_main, font_size=7)
        
        # Базовые вершины - синие
        node_colors = ['#3498db'] * len(self.G.nodes())
        nx.draw_networkx_nodes(self.G, self.pos, ax=self.ax_main, node_color=node_colors, 
                               node_size=600, edgecolors='#2c3e50', linewidths=2)
        
        # Метки вершин
        nx.draw_networkx_labels(self.G, self.pos, ax=self.ax_main, font_size=10, 
                                font_weight='bold', font_color='white')
        
        # Выделение результатов алгоритма
        if self.current_results and self.selected_algorithm:
            results = self.current_results
            
            if self.selected_algorithm == 'dijkstra':
                # Кратчайший путь - красная линия
                vertices = self.graph_data['vertices']
                if len(vertices) > 1:
                    parent = results.get('parent', {})
                    end_vertex = vertices[-1]
                    
                    path = []
                    current = end_vertex
                    while current != -1:
                        path.append(current)
                        current = parent.get(str(current), -1)
                    path.reverse()
                    
                    if path and path[0] == vertices[0]:
                        path_edges = [(path[i], path[i+1]) for i in range(len(path)-1)]
                        nx.draw_networkx_edges(self.G, self.pos, edgelist=path_edges, ax=self.ax_main, 
                                              width=4, edge_color='#e74c3c', alpha=0.9)
                        nx.draw_networkx_nodes(self.G, self.pos, nodelist=path, ax=self.ax_main,
                                              node_color='#e74c3c', node_size=700, edgecolors='#c0392b', linewidths=2.5)
            
            elif self.selected_algorithm == 'tsp':
                # Тур - оранжевая линия
                tour = results.get('tour', [])
                if tour:
                    tour_edges = [(tour[i], tour[(i+1) % len(tour)]) for i in range(len(tour))]
                    nx.draw_networkx_edges(self.G, self.pos, edgelist=tour_edges, ax=self.ax_main,
                                          width=4, edge_color='#f39c12', alpha=0.9)
                    nx.draw_networkx_nodes(self.G, self.pos, nodelist=tour, ax=self.ax_main,
                                          node_color='#f39c12', node_size=700, edgecolors='#e67e22', linewidths=2.5)
            
            elif self.selected_algorithm == 'mst':
                # MST - зелёные рёбра
                mst_edges = results.get('mst_edges', [])
                if mst_edges:
                    mst_edge_list = []
                    for e in mst_edges:
                        if isinstance(e, dict):
                            mst_edge_list.append((e['source'], e['target']))
                        else:
                            mst_edge_list.append((e[0], e[1]))
                    nx.draw_networkx_edges(self.G, self.pos, edgelist=mst_edge_list, ax=self.ax_main,
                                          width=4, edge_color='#2ecc71', alpha=0.9)
            
            elif self.selected_algorithm == 'connectivity':
                # ИСПРАВЛЕНО: Компоненты - разные цвета
                # Даже если граф connected - покрасим в разные цвета для демонстрации
                components = results.get('components', [])
                if components:
                    colors = ['#e74c3c', '#3498db', '#2ecc71', '#f39c12', '#9b59b6', '#1abc9c', '#e67e22', '#34495e']
                    for i, component in enumerate(components):
                        color = colors[i % len(colors)]
                        nx.draw_networkx_nodes(self.G, self.pos, nodelist=component, ax=self.ax_main,
                                              node_color=color, node_size=700, edgecolors='black', linewidths=2)
                    
                    # Если только 1 компонент - подскрасим вершины в разные оттенки
                    if len(components) == 1:
                        component = components[0]
                        colors_gradient = ['#e74c3c', '#e8645b', '#eb7a77', '#ee9091', '#f1a6ab', '#f4bcc5', '#f7d2df']
                        for idx, node in enumerate(component):
                            color = colors_gradient[idx % len(colors_gradient)]
                            nx.draw_networkx_nodes(self.G, self.pos, nodelist=[node], ax=self.ax_main,
                                                  node_color=color, node_size=700, edgecolors='black', linewidths=2)
            
            elif self.selected_algorithm == 'coloring':
                # Раскраска - разные цвета для каждой вершины
                coloring = results.get('coloring', {})
                if coloring and len(coloring) > 0:
                    colors_map = ['#e74c3c', '#3498db', '#2ecc71', '#f39c12', '#9b59b6', '#1abc9c', '#e67e22', '#34495e', '#16a085', '#c0392b']
                    
                    node_colors_dict = {}
                    for node in self.G.nodes():
                        node_key = None
                        if str(node) in coloring:
                            node_key = str(node)
                        elif node in coloring:
                            node_key = node
                        elif int(node) in coloring:
                            node_key = int(node)
                        
                        if node_key is not None:
                            try:
                                color_idx = int(coloring[node_key])
                                node_colors_dict[node] = colors_map[color_idx % len(colors_map)]
                            except:
                                node_colors_dict[node] = '#3498db'
                        else:
                            node_colors_dict[node] = '#3498db'
                    
                    node_list = list(node_colors_dict.keys())
                    colors_final = [node_colors_dict[n] for n in node_list]
                    
                    nx.draw_networkx_nodes(self.G, self.pos, nodelist=node_list, ax=self.ax_main,
                                          node_color=colors_final, node_size=700, edgecolors='black', linewidths=2)
            
            elif self.selected_algorithm == 'hotel':
                # Центры - красные большие вершины
                centers = results.get('centers', [])
                if centers:
                    nx.draw_networkx_nodes(self.G, self.pos, nodelist=centers, ax=self.ax_main,
                                          node_color='#e74c3c', node_size=900, edgecolors='#c0392b', linewidths=3)
        
        title = f"Graph: {self.graph_data['num_vertices']} v, {self.graph_data['num_edges']} e"
        if self.selected_algorithm:
            title += f" | {self.selected_algorithm.upper()}"
        
        self.ax_main.set_title(title, fontsize=12, fontweight='bold')
        self.ax_main.axis('off')
        self.fig.canvas.draw_idle()
    
    def update_info(self):
        """Обновляет информацию"""
        self.ax_info.clear()
        self.ax_info.axis('off')
        
        n = self.graph_data['num_vertices']
        e = self.graph_data['num_edges']
        max_edges = n * (n - 1) // 2
        density = (e / max_edges * 100) if max_edges > 0 else 0
        
        info = f"""GRAPH INFO
─────────────
V: {n}
E: {e}
Type: {self.radio_graph_type.value_selected}
Density: {density:.1f}%"""
        
        if self.current_results and self.selected_algorithm:
            results = self.current_results
            info += "\n\nRESULT\n─────────────"
            
            if self.selected_algorithm == 'dijkstra':
                info += "\nPaths from 0"
            elif self.selected_algorithm == 'tsp':
                info += f"\nTour: {results.get('total_distance', 0):.1f}"
            elif self.selected_algorithm == 'mst':
                info += f"\nWeight: {results.get('total_weight', 0):.1f}"
                info += f"\nConnected: {results.get('is_connected', False)}"
            elif self.selected_algorithm == 'connectivity':
                info += f"\nComponents: {results.get('num_components', 0)}"
            elif self.selected_algorithm == 'coloring':
                info += f"\nColors: {results.get('chromatic_number', 0)}"
            elif self.selected_algorithm == 'hotel':
                info += f"\nCenters: {results.get('num_centers', 0)}"
        
        self.ax_info.text(0.1, 0.9, info, transform=self.ax_info.transAxes, fontsize=10,
                         verticalalignment='top', fontfamily='monospace',
                         bbox=dict(boxstyle='round', facecolor='#ecf0f1', alpha=0.8))
        
        self.fig.canvas.draw_idle()


def main():
    app = GraphVisualizerApp()
    plt.show()


if __name__ == "__main__":
    main()