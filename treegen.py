import json
import networkx as nx
import matplotlib.pyplot as plt
from collections import defaultdict

def load_tree_from_json(file_path):
    """Загружает дерево из JSON, обрабатывает дубликаты узлов."""
    with open(file_path, 'r') as f:
        data = json.load(f)
    
    G = nx.DiGraph()
    node_counter = defaultdict(int)  # Счётчик для каждого имени узла
    node_labels = {}  # Соответствие между ID узла и его исходным именем

    def add_nodes_edges(node, parent=None):
        original_name = node["Item"]["Name"]
        # Создаём уникальный ID: "name_count" (например, "f8_1")
        node_id = f"{original_name}_{node_counter[original_name]}"
        node_counter[original_name] += 1
        
        # Сохраняем исходное имя для подписи
        node_labels[node_id] = original_name
        
        G.add_node(node_id)
        if parent is not None:
            G.add_edge(parent, node_id)
        
        if "Children" in node:
            for child in node["Children"]:
                add_nodes_edges(child, node_id)
    
    add_nodes_edges(data)
    return G, node_labels

def draw_tree(G, node_labels):
    """Визуализирует дерево с исходными именами узлов."""
    pos = nx.drawing.nx_agraph.graphviz_layout(G, prog='dot')
    nx.draw(G, pos, labels=node_labels, with_labels=True, 
            node_size=80, node_color='yellow', arrows=False)
    plt.show()

if __name__ == "__main__":
    G, node_labels = load_tree_from_json("tree.json")
    # print("Узлы дерева (ID -> Имя):", node_labels)
    # print("Рёбра дерева:", G.edges())
    draw_tree(G, node_labels)