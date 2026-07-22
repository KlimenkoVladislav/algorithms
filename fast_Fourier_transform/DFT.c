#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <complex.h>

#define PI 3.14159265358979323846

void DFT(double *amplitudes, double complex *spectrum, int arr_size){
    double complex sum = 0 + 0*I;
    double half_arr_size = arr_size / 2.0;
    int half_arr_plus1 = arr_size / 2 + 1;
    for (int k = 0; k < half_arr_plus1; k++){
        for (int n = 0; n < arr_size; n++){
            sum += amplitudes[n] * cexp((-I * PI * k * n) / half_arr_size);
        }
        if (k != 0){ spectrum[k] = sum / arr_size; }
        else { spectrum[k] = sum * 2 / arr_size; }
        sum = 0.0;
    }
}

void printAll(double *amplitudes, double complex *spectrum, int N, double Fs){
    int half_arr_plus1 = N / 2 + 1;
    double freq;
    double amp;
    printf("\nСпектр:\n");
    for (int k = 0; k < half_arr_plus1; k++) {
        freq = k * Fs / N;
        amp = cabs(spectrum[k]);
        printf("f = %.2f Гц\t|\tАмплитуда = %.4f\n", freq, amp);
    }
}

int main(){
    srand48(time(NULL));

    int Fs = 1000;
    double time;
    do {
        printf("Введите время записи сигнала: ");
        scanf("%le", &time);
    } while (time < 0.01);
    int arr_size = Fs * time;

    double *amplitudes = malloc(arr_size * sizeof(double));
    if (amplitudes == NULL){printf("Память не выделена\n"); return -1;}
    for (int i = 0; i < arr_size; i++){
        amplitudes[i] = -10000 + drand48() * 20000;
    }
    
    double complex *spectrum = malloc((arr_size / 2 + 1) * sizeof(double complex));
    if (spectrum == NULL){printf("Память не выделена\n"); return -1;}
    DFT(amplitudes, spectrum, arr_size);

    printAll(amplitudes, spectrum, arr_size, Fs);

    free(amplitudes); amplitudes = NULL;
    free(spectrum); spectrum = NULL;

    return 0;
}