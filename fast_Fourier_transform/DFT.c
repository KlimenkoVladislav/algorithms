#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <complex.h>

#define PI 3.14159265358979323846

void DFT(double *amplitudes, double complex *spectrum, int arr_size){
    double complex sum = 0 + 0*I;
    double half_arr_size = arr_size / 2.0;
    for (int k = 0; k < half_arr_size + 0.5; k++){
        for (int n = 0; n < arr_size; n++){
            sum += amplitudes[n] * cexp((-I * PI * k * n) / half_arr_size);
        }
        spectrum[k] = sum;
        sum = 0.0;
    }
}

void printAll(double *amplitudes, double complex *spectrum, int arr_size) {
    printf("\nСигнал (время): [");
    for (int i = 0; i < arr_size; i++) {
        printf("%.4f", amplitudes[i]);
        if (i < arr_size - 1) printf(", ");
    }
    printf("]\n");

    // Печать только полезных частот
    int half = arr_size / 2;
    printf("Спектр (частота): [");
    for (int k = 0; k < half + 1; k++) {
        printf("%.4f %+.4fi", creal(spectrum[k]), cimag(spectrum[k]));
        if (k < half) printf(", ");
    }
    printf("]\n");
}

int main(){
    srand48(time(NULL));

    int arr_size;
    do {
        printf("Введите количество элементов в массиве амплитуд: ");
        scanf("%d", &arr_size);
    } while (arr_size < 1);

    double *amplitudes = malloc(arr_size * sizeof(double));
    if (amplitudes == NULL){printf("Память не выделена\n"); return -1;}
    for (int i = 0; i < arr_size; i++){
        amplitudes[i] = -10000 + drand48() * 20000;
    }
    
    double complex *spectrum = malloc((arr_size / 2 + 1) * sizeof(double complex));
    if (spectrum == NULL){printf("Память не выделена\n"); return -1;}
    DFT(amplitudes, spectrum, arr_size);

    printAll(amplitudes, spectrum, arr_size);

    free(amplitudes); amplitudes = NULL;
    free(spectrum); spectrum = NULL;

    return 0;
}