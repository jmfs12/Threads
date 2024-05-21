#define _XOPEN_SOURCE 600

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int n = 0;
int *arr;
pthread_barrier_t barrier;

void *bubblesort(void *threadid);
void merge(void *threadid);

int main(){
    int N, len;
    printf("Insira a quantidade de partes para dividir o array: ");
    scanf("%d", &N);
    printf("\nIndique o tamanho do array para ser gerado de forma aleatória: ");
    scanf("%d",&len);
    *arr = (int*)malloc(len*sizeof(int);
    printf("\nO array gerado, em ordem é: "\n);
    for (int i = 0; i < len; i++) {
        arr[i] = rand() % 100;
        printf("%d ",array[i]);
    }

    int n = len/N;

    pthread_t *threads = (pthread_t *)malloc(n * sizeof(pthread_t));
    if(threads == NULL) { printf("Erro na alocacao1\n"); exit(1); };
    int **taskids = (int **)malloc((n+1) * sizeof(int*));
    if(taskids == NULL) { printf("Erro na alocacao2\n"); free(threads); exit(1); };
    pthread_t lastThread;

    pthread_barrier_init(&barrier, NULL, n);

    for(int i = 0; i < n; i++){
        taskids[i] = (int *)malloc(sizeof(int));
        if(taskids[i] == NULL) { printf("Erro na alocacao3"); free(threads); free(taskids); exit(1); }
        *taskids[i] = i;
        pthread_create(&threads[i], NULL, bubblesort, (void *) taskids[i]);
    }
    taskids[n] = (int *)malloc(sizeof(int));
    *taskids[n] = n;

    pthread_create(&lastThread, NULL, merge, (void *)taskids[n]);


    for(int i = 0; i < n; i++){
        pthread_join(threads[i], NULL);
    }
    pthread_barrier_wait(&barrier);

    pthread_join(lastThread, NULL);

    pthread_barrier_destroy(&barrier);
    
    pthread_exit(NULL);
}

void *bubblesort(void *threadid){
    for(int i = (int)threadid*n; i < ((int)threadid+1)*n; i++){
        for(int j = (int)threadid*n; j < (((int)threadid+1)*n)-1-(i-(int)threadid*n);j++){
            if(arr[j] > arr[j+1]){
                int tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
        }
    }
    pthread_barrier_wait(&barrier);

    pthread_exit(NULL);
}

void merge(void *threadid){
    
}
