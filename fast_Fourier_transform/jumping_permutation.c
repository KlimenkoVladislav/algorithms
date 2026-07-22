#include <stdio.h>
#include <stdlib.h>

void jumpingPermutations(int *arr, int n){
    int index = 1;
    int a = n >> 1;
    int b = a >> 1;
    int bound = b >> 1;
    int half_bound = bound >> 1;
    int prev = 0;
    int anomaly = 0;
    int temp;
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

int main(){
    int k;
    printf("Введите степень двойки: "); scanf("%d", &k);

    int n = 1;
    for (int i = 0; i < k; i++){
        n *= 2;
    }

    int *arr = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++){
        arr[i] = i;
    }

    jumpingPermutations(arr, n);

    for (int i = 0; i < n; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}