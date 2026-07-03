#include <iostream>
#include <vector>
#include <fstream>
#include <random>

#define RANDOM_VECTOR_SIZE 1000
#define DB_SIZE 10000

void QuickSort(std::vector<int> &vector_of_nonzero_indices, 
               int left, int right){
    int X = vector_of_nonzero_indices[left];
    int i = left;
    int j = right;
    while (i<=j){
        while (vector_of_nonzero_indices[i] < X){ i++; }
        while (vector_of_nonzero_indices[j] > X){ j--; }
        if (i <= j){
            std::swap(vector_of_nonzero_indices[i],
                vector_of_nonzero_indices[j]);
                i++;
                j--;
            }
    }
    if (left < j){ QuickSort(vector_of_nonzero_indices, left, j); }
    if (i < right){ QuickSort(vector_of_nonzero_indices, i, right); }
}

void fillingWithRandomVectors(){
    std::ofstream file("database.db", std::ios::out);
    if (!file.is_open()){
        std::cout << "Не удалось открыть файл\n";
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist1(1, 150);

    for (int i = 0; i < DB_SIZE; i++){
        std::vector<int> random_vector(RANDOM_VECTOR_SIZE);
        
        int random_number_of_nonzero_koordinats = dist1(gen);
        std::vector<int> vector_of_nonzero_indices(random_number_of_nonzero_koordinats);

        std::uniform_int_distribution<int> dist2(0, RANDOM_VECTOR_SIZE-1);
        for (int j = 0; j < random_number_of_nonzero_koordinats; j++){
            vector_of_nonzero_indices[j] = dist2(gen);
        }
        QuickSort(vector_of_nonzero_indices, 0, random_number_of_nonzero_koordinats-1);

        int pointer_to_index = 0;
        for (int j = 0; j < RANDOM_VECTOR_SIZE; j++){
            if ((pointer_to_index < random_number_of_nonzero_koordinats) and 
                (j == vector_of_nonzero_indices[pointer_to_index])){
                random_vector[j] = dist1(gen);
                pointer_to_index++;
            }
            else { random_vector[j] = 0; }
        }

        for (int j = 0; j < RANDOM_VECTOR_SIZE; j++){
            file << random_vector[j] << " ";
        }
        file << "\n";
    }

    file.close();
}

int main(){
    fillingWithRandomVectors();
}