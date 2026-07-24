import matplotlib.pyplot as plt
import numpy as np
from collections import defaultdict

def read_data(filename):
    data = defaultdict(lambda: {'jumping': [], 'classic': []})
    
    with open(filename, 'r') as f:
        lines = f.readlines()
    
    start = 0
    if lines and lines[0].strip().startswith('N'):
        start = 1
    
    for line in lines[start:]:
        line = line.strip()
        if not line:
            continue
        parts = line.split()
        if len(parts) < 3:
            continue
        try:
            n = int(parts[0])
            time_jumping = float(parts[1]) * 1000.0
            time_classic = float(parts[2]) * 1000.0
            data[n]['jumping'].append(time_jumping)
            data[n]['classic'].append(time_classic)
        except ValueError:
            continue
    
    return data

def plot_results(data):
    ns = sorted(data.keys())
    
    if not ns:
        print("Нет данных для построения!")
        return
    
    chunk_size = 6
    chunks = [ns[i:i+chunk_size] for i in range(0, len(ns), chunk_size)]
    
    for idx, chunk in enumerate(chunks):
        jumping_means = [np.mean(data[n]['jumping']) for n in chunk]
        classic_means = [np.mean(data[n]['classic']) for n in chunk]
        jumping_stds = [np.std(data[n]['jumping']) for n in chunk]
        classic_stds = [np.std(data[n]['classic']) for n in chunk]
        
        fig, ax = plt.subplots(figsize=(10, 6))
        
        ax.errorbar(chunk, jumping_means, yerr=jumping_stds,
                    fmt='o-', color='red', linewidth=2, markersize=8,
                    capsize=5, label='jumpingPermutations (ваш алгоритм)')
        
        ax.errorbar(chunk, classic_means, yerr=classic_stds,
                    fmt='s-', color='blue', linewidth=2, markersize=8,
                    capsize=5, label='bit_reverse_classic')
        
        ax.set_xscale('log', base=2)
        ax.set_xlabel('Размер массива (N)', fontsize=14)
        ax.set_ylabel('Время выполнения (мс)', fontsize=14)
        ax.set_title(f'Сравнение скорости бит-реверсии (N = {chunk[0]} ... {chunk[-1]})', fontsize=16)
        ax.grid(True, alpha=0.3)
        ax.legend(fontsize=12)
        
        ax.set_xticks(chunk)
        ax.set_xticklabels([str(n) for n in chunk], rotation=45)
        
        # ===== ПОДПИСИ: КРАСНОЕ ПОД КРАСНЫМ, СИНЕЕ НАД СИНИМ =====
        for n, jm, cm in zip(chunk, jumping_means, classic_means):
            # Красное значение — ПОД точкой (смещение вниз)
            ax.annotate(f'{jm:.6f}', (n, jm), 
                       textcoords="offset points", xytext=(0, -15), 
                       ha='center', fontsize=9, color='red')
            # Синее значение — НАД точкой (смещение вверх)
            ax.annotate(f'{cm:.6f}', (n, cm), 
                       textcoords="offset points", xytext=(0, 12), 
                       ha='center', fontsize=9, color='blue')
        
        plt.tight_layout()
        plt.savefig(f'benchmark_results_{idx+1}.png', dpi=300)
        plt.show()
    
    # Сводная таблица
    print("\n=== Сводная таблица (в миллисекундах) ===")
    print(f"{'N':>8} | {'jumpingPermutations':>20} | {'bit_reverse_classic':>20} | {'Ускорение':>10}")
    print("-" * 75)
    for n in ns:
        jm = np.mean(data[n]['jumping'])
        cm = np.mean(data[n]['classic'])
        speedup = cm / jm if jm > 0 else 0
        print(f"{n:>8} | {jm:>20.6f} | {cm:>20.6f} | {speedup:>10.2f}x")

def main():
    filename = 'results_O3.txt'
    
    try:
        data = read_data(filename)
    except FileNotFoundError:
        print(f"Ошибка: файл '{filename}' не найден!")
        return
    
    if not data:
        print("Файл пуст или не содержит данных!")
        return
    
    ns = sorted(data.keys())
    print(f"Найдено {len(ns)} различных размеров N: {ns}")
    
    plot_results(data)

if __name__ == "__main__":
    main()