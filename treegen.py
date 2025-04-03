# import json
# import networkx as nx
# import matplotlib.pyplot as plt
# from collections import defaultdict

# def load_tree_from_json(file_path):
#     """Загружает дерево из JSON, обрабатывает дубликаты узлов."""
#     with open(file_path, 'r') as f:
#         data = json.load(f)
    
#     G = nx.DiGraph()
#     node_counter = defaultdict(int)  # Счётчик для каждого имени узла
#     node_labels = {}  # Соответствие между ID узла и его исходным именем

#     def add_nodes_edges(node, parent=None):
#         original_name = node["Item"]["Name"]
#         # Создаём уникальный ID: "name_count" (например, "f8_1")
#         node_id = f"{original_name}_{node_counter[original_name]}"
#         node_counter[original_name] += 1
        
#         # Сохраняем исходное имя для подписи
#         node_labels[node_id] = original_name
        
#         G.add_node(node_id)
#         if parent is not None:
#             G.add_edge(parent, node_id)
        
#         if "Children" in node:
#             for child in node["Children"]:
#                 add_nodes_edges(child, node_id)
    
#     add_nodes_edges(data)
#     return G, node_labels

# def draw_tree(G, node_labels):
#     """Визуализирует дерево с исходными именами узлов."""
#     pos = nx.drawing.nx_agraph.graphviz_layout(G, prog='dot')
#     nx.draw(G, pos, labels=node_labels, with_labels=True, 
#             node_size=80, node_color='yellow', arrows=False)
#     plt.show()

# if __name__ == "__main__":
#     G, node_labels = load_tree_from_json("tree.json")
#     # print("Узлы дерева (ID -> Имя):", node_labels)
#     # print("Рёбра дерева:", G.edges())
#     draw_tree(G, node_labels)



# import json
# from graphviz import Digraph
# import networkx as nx

# def load_tree_from_json(file_path):
#     """Загружает дерево из JSON и возвращает граф NetworkX."""
#     with open(file_path, 'r') as f:
#         data = json.load(f)
    
#     G = nx.DiGraph()
#     node_counter = defaultdict(int)
#     node_labels = {}

#     def add_nodes_edges(node, parent=None):
#         original_name = node["Item"]["Name"]
#         node_id = f"{original_name}_{node_counter[original_name]}"
#         node_counter[original_name] += 1
#         node_labels[node_id] = original_name
        
#         G.add_node(node_id)
#         if parent is not None:
#             G.add_edge(parent, node_id)
        
#         if "Children" in node:
#             for child in node["Children"]:
#                 add_nodes_edges(child, node_id)
    
#     add_nodes_edges(data)
#     return G, node_labels

# def export_to_graphviz(G, node_labels, output_file="tree"):
#     """Экспортирует дерево в GraphViz (PDF/SVG/PNG)."""
#     dot = Digraph(comment="Tree", format="png", engine="dot")
#     dot.attr(rankdir="TB", nodesep="0.5", ranksep="1.0")  # TB — сверху вниз

#     # Добавляем узлы
#     for node_id in G.nodes():
#         dot.node(node_id, label=node_labels[node_id], shape="box", fontsize="10")

#     # Добавляем рёбра
#     for edge in G.edges():
#         dot.edge(edge[0], edge[1])

#     # Сохраняем и рендерим
#     dot.render(output_file, cleanup=True)
#     print(f"Дерево сохранено в {output_file}.png")

# if __name__ == "__main__":
#     from collections import defaultdict
#     G, node_labels = load_tree_from_json("tree.json")
#     export_to_graphviz(G, node_labels)

import json
from graphviz import Digraph
from collections import defaultdict
import networkx as nx

def load_tree_from_json(file_path):
    with open(file_path, 'r') as f:
        data = json.load(f)
    G = nx.DiGraph()
    node_counter = defaultdict(int)
    node_labels = {}

    def add_nodes_edges(node, parent=None):
        original_name = node["Item"]["Name"]
        node_id = f"{original_name}_{node_counter[original_name]}"
        node_counter[original_name] += 1
        node_labels[node_id] = original_name
        G.add_node(node_id)
        if parent is not None:
            G.add_edge(parent, node_id)
        if "Children" in node:
            for child in node["Children"]:
                add_nodes_edges(child, node_id)
    
    add_nodes_edges(data)
    return G, node_labels

def export_to_graphviz(G, node_labels, output_file="tree"):
    dot = Digraph(engine="neato") # //FIXME
    dot.attr(rankdir="LR", 
           nodesep="0.3",
           ranksep="1.5",
           size="50,30",
           concentrate="false")  # Отключаем слияние рёбер

    for node_id in G.nodes():
        dot.node(node_id, label=node_labels[node_id], 
                shape="ellipse", fontsize="10", width="0.5")

    for edge in G.edges():
        dot.edge(edge[0], edge[1], arrowsize="0.5")

    dot.render(output_file, format="svg", cleanup=True) # //FIXME
    print(f"Сохранено в {output_file}.png")

if __name__ == "__main__":
    G, node_labels = load_tree_from_json("tree.json")
    export_to_graphviz(G, node_labels)