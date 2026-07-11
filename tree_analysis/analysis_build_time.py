import matplotlib.pyplot as plt

# Чтение данных из файла
def read_data(filename):
    max_vectors = []
    times_ms = []
    
    with open(filename, 'r') as file:
        for line in file:
            if line.strip():
                parts = line.strip().split()
                if len(parts) == 2:
                    max_vectors.append(float(parts[0]))
                    times_ms.append(float(parts[1]))
    
    return max_vectors, times_ms

# Построение графика
max_vectors, times_ms = read_data('results2.txt')

plt.figure(figsize=(10, 6))
plt.plot(max_vectors, times_ms, 'bo-', linewidth=2, markersize=8)
plt.xlabel('Максимальное количество векторов в листе')
plt.ylabel('Время построения (мс)')
plt.title('Зависимость времени построения k-d tree от max векторов в листе')
plt.grid(True, alpha=0.3)
plt.show()