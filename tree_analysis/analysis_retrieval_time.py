import matplotlib.pyplot as plt
import numpy as np

def read_data(filename):
    """Чтение данных из файла"""
    data = []
    with open(filename, 'r') as f:
        for line in f:
            parts = line.strip().split()
            if len(parts) == 4:
                db_size, leaf_size, k, time = map(float, parts)
                data.append((int(db_size), int(leaf_size), int(k), time))
    return data

def plot_time_vs_leaf_size(data):
    """Построение графиков зависимости времени от размера листа"""
    # Группируем данные по размеру базы данных и k
    grouped = {}
    for db_size, leaf_size, k, time in data:
        key = (db_size, k)
        if key not in grouped:
            grouped[key] = {'leaf_sizes': [], 'times': []}
        grouped[key]['leaf_sizes'].append(leaf_size)
        grouped[key]['times'].append(time)
    
    # Сортируем данные для каждого графика
    for key in grouped:
        leaf_sizes = grouped[key]['leaf_sizes']
        times = grouped[key]['times']
        sorted_pairs = sorted(zip(leaf_sizes, times))
        grouped[key]['leaf_sizes'] = [x[0] for x in sorted_pairs]
        grouped[key]['times'] = [x[1] for x in sorted_pairs]
    
    # Получаем уникальные размеры базы данных
    db_sizes = sorted(set(db_size for db_size, _ in grouped.keys()))
    
    # Для каждого размера базы создаём отдельный график
    for db_size in db_sizes:
        plt.figure(figsize=(10, 6))
        
        # Фильтруем данные для текущего размера базы
        db_data = {k: grouped[(db_size, k)] for k in 
                   sorted(set(k for db, k in grouped.keys() if db == db_size))}
        
        # Находим все уникальные размеры листьев для этого графика
        all_leaf_sizes = sorted(set(
            leaf_size for values in db_data.values() 
            for leaf_size in values['leaf_sizes']
        ))
        
        # Рисуем линии для разных k
        colors = plt.cm.tab10(np.linspace(0, 1, len(db_data)))
        for idx, (k, values) in enumerate(db_data.items()):
            leaf_sizes = values['leaf_sizes']
            times = values['times']
            plt.plot(leaf_sizes, times, marker='o', linewidth=2, 
                    markersize=8, color=colors[idx], label=f'k={k}')
            
            # Подписываем значения
            for x, y in zip(leaf_sizes, times):
                plt.annotate(f'{y:.3f}', (x, y), textcoords="offset points", 
                           xytext=(0, 10), ha='center', fontsize=9)
        
        # Используем логарифмическую шкалу по X, чтобы видеть все значения
        if len(all_leaf_sizes) > 3:
            plt.xscale('log')
            plt.xlabel('Количество векторов в листе (логарифмическая шкала)', fontsize=12)
        else:
            plt.xlabel('Количество векторов в листе', fontsize=12)
        
        plt.ylabel('Время поиска (мс)', fontsize=12)
        plt.title(f'База данных: {db_size} векторов', fontsize=14, fontweight='bold')
        plt.legend(loc='best', fontsize=10)
        plt.grid(True, alpha=0.3, linestyle='--', which='both')
        
        plt.tight_layout()
        plt.show()

def plot_all_in_one(data):
    """Все данные на одном графике"""
    plt.figure(figsize=(14, 8))
    
    # Группируем по размеру базы и k
    grouped = {}
    for db_size, leaf_size, k, time in data:
        key = (db_size, k)
        if key not in grouped:
            grouped[key] = {'leaf_sizes': [], 'times': []}
        grouped[key]['leaf_sizes'].append(leaf_size)
        grouped[key]['times'].append(time)
    
    # Сортируем
    for key in grouped:
        sorted_pairs = sorted(zip(grouped[key]['leaf_sizes'], grouped[key]['times']))
        grouped[key]['leaf_sizes'] = [x[0] for x in sorted_pairs]
        grouped[key]['times'] = [x[1] for x in sorted_pairs]
    
    # Получаем уникальные размеры базы
    db_sizes = sorted(set(db_size for db_size, _ in grouped.keys()))
    
    # Разные стили для разных размеров базы
    markers = ['o', 's', '^', 'D', 'v']
    linestyles = ['-', '--', '-.', ':']
    colors = plt.cm.Set1(np.linspace(0, 1, len(db_sizes)))
    
    # Для каждого размера базы создаём отдельные линии для разных k
    for i, db_size in enumerate(db_sizes):
        db_data = {k: grouped[(db_size, k)] for k in 
                   sorted(set(k for db, k in grouped.keys() if db == db_size))}
        
        for j, (k, values) in enumerate(db_data.items()):
            leaf_sizes = values['leaf_sizes']
            times = values['times']
            label = f'DB={db_size}, k={k}'
            plt.plot(leaf_sizes, times, 
                    marker=markers[j % len(markers)], 
                    linestyle=linestyles[i % len(linestyles)], 
                    color=colors[i], 
                    linewidth=2, 
                    markersize=8, 
                    alpha=0.8,
                    label=label)
    
    # Логарифмическая шкала по X
    plt.xscale('log')
    plt.xlabel('Количество векторов в листе (логарифмическая шкала)', fontsize=12)
    plt.ylabel('Время поиска (мс)', fontsize=12)
    plt.title('Зависимость времени поиска от размера листа\n(разные размеры базы данных)', fontsize=14, fontweight='bold')
    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left', fontsize=9)
    plt.grid(True, alpha=0.3, linestyle='--', which='both')
    plt.tight_layout()
    plt.show()

def plot_by_k(data):
    """Графики для каждого k отдельно"""
    # Группируем по k и размеру базы
    grouped = {}
    for db_size, leaf_size, k, time in data:
        key = (k, db_size)
        if key not in grouped:
            grouped[key] = {'leaf_sizes': [], 'times': []}
        grouped[key]['leaf_sizes'].append(leaf_size)
        grouped[key]['times'].append(time)
    
    # Сортируем
    for key in grouped:
        sorted_pairs = sorted(zip(grouped[key]['leaf_sizes'], grouped[key]['times']))
        grouped[key]['leaf_sizes'] = [x[0] for x in sorted_pairs]
        grouped[key]['times'] = [x[1] for x in sorted_pairs]
    
    # Получаем уникальные k
    k_values = sorted(set(k for k, _ in grouped.keys()))
    
    # Для каждого k создаём отдельный график
    for k in k_values:
        plt.figure(figsize=(10, 6))
        
        # Фильтруем данные для текущего k
        k_data = {db_size: grouped[(k, db_size)] for db_size in 
                  sorted(set(db_size for _, db_size in grouped.keys() if k == k))}
        
        colors = plt.cm.viridis(np.linspace(0, 1, len(k_data)))
        for i, (db_size, values) in enumerate(k_data.items()):
            leaf_sizes = values['leaf_sizes']
            times = values['times']
            plt.plot(leaf_sizes, times, marker='o', linewidth=2, 
                    markersize=8, color=colors[i], label=f'DB={db_size}')
            
            # Подписываем значения
            for x, y in zip(leaf_sizes, times):
                plt.annotate(f'{y:.3f}', (x, y), textcoords="offset points", 
                           xytext=(0, 5), ha='center', fontsize=8)
        
        # Логарифмическая шкала по X
        plt.xscale('log')
        plt.xlabel('Количество векторов в листе (логарифмическая шкала)', fontsize=12)
        plt.ylabel('Время поиска (мс)', fontsize=12)
        plt.title(f'k = {k} (ищем {k} похожих векторов)', fontsize=14, fontweight='bold')
        plt.legend(loc='best', fontsize=10)
        plt.grid(True, alpha=0.3, linestyle='--', which='both')
        plt.tight_layout()
        plt.show()

# Основная программа
if __name__ == "__main__":
    # Читаем данные
    data = read_data('results.txt')
    
    print("📊 Строим графики...")
    
    # Строим графики
    plot_time_vs_leaf_size(data)  # Отдельные графики для каждой базы
    plot_all_in_one(data)          # Все на одном графике
    plot_by_k(data)               # Графики для каждого k
    
    print("✅ Все графики построены!")