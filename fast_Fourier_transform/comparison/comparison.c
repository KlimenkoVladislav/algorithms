#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <time.h>

void jumpingPermutations(double complex *arr, int n){
    int index = 1;
    int a = n >> 1;
    int b = a >> 1;
    int bound = b >> 1;
    int half_bound = bound >> 1;
    int prev = 0;
    int anomaly = 0;
    double complex temp;
    int mirror_index;
    int mirror_prev;
    n--;
    for (int i = 0; i < half_bound; i++){
        prev += a;
        temp = arr[prev];
        arr[prev] = arr[index];
        arr[index] = temp;
        index++;
    
        prev -= b;
        temp = arr[prev];
        arr[prev] = arr[index];
        arr[index] = temp;
        mirror_prev = n - prev;
        mirror_index = n - index;
        temp = arr[mirror_prev];
        arr[mirror_prev] = arr[mirror_index];
        arr[mirror_index] = temp;
        index++;
    
        prev += a;
        temp = arr[prev];
        arr[prev] = arr[index];
        arr[index] = temp;
        index += 2;

        anomaly += 4;
        prev = anomaly;
    }
    anomaly = 2;
    prev = anomaly;
    for (int i = half_bound; i < bound; i++){
        prev += a;
        temp = arr[prev];
        arr[prev] = arr[index];
        arr[index] = temp;
        index += 2;
    
        prev -= b;
    
        prev += a;
        temp = arr[prev];
        arr[prev] = arr[index];
        arr[index] = temp;
        index += 2;

        anomaly += 4;
        prev = anomaly;
    }
}

void bit_reverse_classic(double complex *arr, int n) {
    int j = 0;
    for (int i = 0; i < n - 1; i++) {
        int k = n >> 1;
        while (k <= j) {
            j -= k;
            k >>= 1;
        }
        j += k;
        
        if (i < j) {
            double complex temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
}

double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

void copy_array(double complex *dest, double complex *src, int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

int main() {
    int k;
    printf("Введите степень двойки (например, 10 для 1024): ");
    scanf("%d", &k);

    int n = 1 << k;

    FILE *file = fopen("results_O3.txt", "a");
    if (file == NULL) {
        printf("Ошибка открытия файла!\n");
        return -1;
    }

    double complex *original = malloc(n * sizeof(double complex));
    double complex *arr1 = malloc(n * sizeof(double complex));
    double complex *arr2 = malloc(n * sizeof(double complex));
    
    if (original == NULL || arr1 == NULL || arr2 == NULL) {
        printf("Ошибка выделения памяти!\n");
        return -1;
    }

    srand48(time(NULL));
    for (int i = 0; i < n; i++) {
        double real = -10000 + drand48() * 20000;
        double imag = -10000 + drand48() * 20000;
        original[i] = real + imag * I;
    }

    int iterations = 250;

    for (int iter = 1; iter <= iterations; iter++) {
        if (iter % 10 == 0 || iter == 1) {
            printf("Итерация %d из %d...\n", iter, iterations);
        }

        copy_array(arr1, original, n);
        double start = get_time();
        jumpingPermutations(arr1, n);
        double end = get_time();
        double time_jumping = end - start;

        copy_array(arr2, original, n);
        start = get_time();
        bit_reverse_classic(arr2, n);
        end = get_time();
        double time_classic = end - start;

        fprintf(file, "%d\t%.9f\t%.9f\n", n, time_jumping, time_classic);
    }

    fclose(file);
    free(original);
    free(arr1);
    free(arr2);

    return 0;
}