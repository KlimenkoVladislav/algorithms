#include <stdio.h>
#include <stdlib.h>

void jumpingPermutations(int *arr, int n, int k){
    int index = 1;
    int a = n >> 1;
    int b = a >> 1;
    int bound = b >> 1;
    n--;
    int prev = 0;
    int temp;
    int mirror_index;
    int mirror_prev;
    for (int i = 0; i < bound; i++){
        prev += a;
        temp = arr[prev];
        arr[prev] = arr[index];
        arr[index] = temp;
        index++;
    
        prev -= b;
        if (index < b){
            temp = arr[prev];
            arr[prev] = arr[index];
            arr[index] = temp;

            mirror_prev = n - prev;
            mirror_index = n - index;
            temp = arr[mirror_prev];
            arr[mirror_prev] = arr[mirror_index];
            arr[mirror_index] = temp;
        }
        index++;
    
        prev += a;
        temp = arr[prev];
        arr[prev] = arr[index];
        arr[index] = temp;
        index += 2;

        prev = arr[index - 1];
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

    jumpingPermutations(arr, n, k);

    for (int i = 0; i < n; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}