import json
from pathlib import Path
import hashlib
from contextlib import contextmanager

@contextmanager
def nullcontext():
    yield None

def visualize_tree(input_json, output_file=None, max_depth=4, max_children=500, min_sequence=3):
    """Визуализация с правильной сортировкой и сворачиванием"""
    
    def hex_key(name):
        """Ключ сортировки для шестнадцатеричных значений"""
        try:
            clean = name.split('_')[0].lower().lstrip('0x').rstrip('h')
            return int(clean, 16) if clean else 0
        except:
            return float('inf')
    
    def natural_sort_key(node):
        """Ключ для натуральной сортировки HEX-значений"""
        name = node["Item"]["Name"].split('_')[0]
        return (hex_key(name), name)
    
    def get_child_fingerprint(node):
        """Хеш структуры потомков"""
        if "Children" not in node:
            return ""
        return hashlib.sha256(
            json.dumps(node["Children"], 
                      sort_keys=True,
                      ensure_ascii=False).encode()
        ).hexdigest()
    
    def process_level(nodes, level):
        """Обработка уровня с правильной сортировкой и группировкой"""
        if level != max_depth - 2:
            return nodes
            
        # Сортируем узлы по натуральному HEX-порядку
        sorted_nodes = sorted(nodes, key=natural_sort_key)
        
        groups = []
        current_group = []
        prev_fp = None
        prev_num = None
        
        for node in sorted_nodes:
            current_fp = get_child_fingerprint(node)
            current_num = hex_key(node["Item"]["Name"])
            
            # Условия для продолжения последовательности:
            # 1. Совпадает структура потомков
            # 2. Числа идут последовательно
            if (current_fp == prev_fp and 
                current_fp != "" and 
                prev_num is not None and 
                current_num == prev_num + 1):
                current_group.append(node)
            else:
                if current_group:
                    groups.append(current_group)
                current_group = [node]
                prev_fp = current_fp
            prev_num = current_num
            
        if current_group:
            groups.append(current_group)
            
        # Формируем итоговый список с разделителями
        result = []
        for group in groups:
            if len(group) >= min_sequence:
                result.append(group[0])
                result.append({"Item": {"Name": "|"}})
                result.append(group[-1])
            else:
                result.extend(group)
                
        return result
    
    def format_ranges(addresses):
        """Форматирование диапазонов с правильной сортировкой"""
        sorted_addrs = sorted(addresses, key=hex_key)
        ranges = []
        if not sorted_addrs:
            return ranges
            
        start = end = sorted_addrs[0]
        
        for addr in sorted_addrs[1:]:
            if hex_key(addr) == hex_key(end) + 1:
                end = addr
            else:
                ranges.append(f"{start}..{end}" if start != end else start)
                start = end = addr
        ranges.append(f"{start}..{end}" if start != end else start)
        
        return ranges
    
    def traverse(node, level=0, output=None):
        """Обход дерева с улучшенным выводом"""
        name = node["Item"]["Name"].split('_')[0]
        children = node.get("Children", [])
        
        line = "   " * level
        
        # Обработка последнего уровня
        if level == max_depth - 1:
            if children:
                addrs = [c["Item"]["Name"].split('_')[0] for c in children]
                ranges = format_ranges(addrs)
                line += f"{name} = [{', '.join(ranges)}]"
            else:
                line += name
            print(line)
            if output: output.write(line + "\n")
            return
        
        # Обработка предпоследнего уровня
        if level == max_depth - 2:
            children = process_level(children, level)
        
        # Вывод текущего узла
        line += f"{name}{' ┐' if children else ''}"
        print(line)
        if output: output.write(line + "\n")
        
        # Ограничение глубины
        if level >= max_depth:
            return
            
        # Рекурсивная обработка детей
        shown = 0
        for child in children:
            if shown >= max_children:
                break
                
            if child["Item"]["Name"] == "|":
                line = "   " * (level + 1) + "|"
                print(line)
                if output: output.write(line + "\n")
                shown += 1
            else:
                traverse(child, level + 1, output)
                shown += 1
                
        if len(children) > max_children:
            line = "   " * (level + 1) + f"... ({len(children)} total)"
            print(line)
            if output: output.write(line + "\n")

    # Загрузка данных
    data = json.loads(Path(input_json).read_text())
    
    # Вывод результата
    with open(output_file, 'w') if output_file else nullcontext() as f:
        print("\nДерево с правильной сортировкой:")
        if f: f.write("Дерево с правильной сортировкой:\n")
        traverse(data, output=f if output_file else None)

if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description='Визуализатор деревьев')
    parser.add_argument("input_json", help="JSON-файл с деревом")
    parser.add_argument("-o", "--output", help="Файл для сохранения")
    parser.add_argument("-d", "--depth", type=int, default=4, 
                      help="Глубина отображения")
    parser.add_argument("-c", "--children", type=int, default=500,
                      help="Лимит детей на уровне")
    parser.add_argument("-s", "--sequence", type=int, default=3,
                      help="Мин. длина последовательности")
    
    args = parser.parse_args()
    
    visualize_tree(
        input_json=args.input_json,
        output_file=args.output,
        max_depth=args.depth,
        max_children=args.children,
        min_sequence=args.sequence
    )