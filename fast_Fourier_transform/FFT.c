#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <complex.h>

#define PI 3.14159265358979323846

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

// только если количество элементов в массиве - степень двойки
void FFT(double complex *arr, int n){
    int parts = n;
    double complex temp;
    for (int i = 0; i < parts; i += 2){
        temp = arr[i];
        arr[i] += arr[i+1];
        arr[i+1] = temp - arr[i+1];
    }
    parts >>= 1;
    int len_1_part = 2;
    double complex W;
    for ( ; parts != 1; parts >>= 1, len_1_part <<= 1){
        for (int k = 0; k < len_1_part; k++){
            W = cexp(-I * PI * k / len_1_part);
            for (int i = k; i < n; i += 2*len_1_part){
                temp = arr[i];
                arr[i] += W * arr[i + len_1_part];
                arr[i + len_1_part] = temp - W * arr[i + len_1_part];
            }
        }
    }
}

void printSpectrum(double complex *arr, int N, double Fs) {
    int half = N / 2;
    printf("\nСпектр (амплитуды):\n");
    for (int k = 0; k < half + 1; k++) {
        double freq = (double)k * Fs / N;
        double amp = cabs(arr[k]);
        
        if (k == 0) { amp /= N; } 
        else{ amp /= (N / 2.0); }
        
        printf("f = %.2f Гц\t|\tАмплитуда = %.4f\n", freq, amp);
    }
}

int main(){
    srand48(time(NULL));

    int Fs = 1024;
    double time;
    do {
        printf("Введите время записи сигнала: ");
        scanf("%le", &time);
    } while (time < 0.01);
    int arr_size = Fs * time;

    double real;
    double complex *arr = malloc(arr_size * sizeof(double complex));
    if (arr == NULL){ printf("Память не выделена\n"); return -1; }
    for (int i = 0; i < arr_size; i++){
        real = -10000 + drand48() * 20000;
        arr[i] = real + 0.0*I;
    }

    jumpingPermutations(arr, arr_size);
    FFT(arr, arr_size);
    printSpectrum(arr, arr_size, Fs);

    return 0;
}