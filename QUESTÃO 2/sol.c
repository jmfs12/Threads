/*2. Crie um programa em C que utilize o algoritmo de ordenação bubble sort de maneira concorrente. 
O programa deve dividir um array em N partes, e cada thread deve ordenar uma parte. Após todas as 
threads completarem a ordenação de suas respectivas partes, uma thread final deve mesclar todos os
segmentos para formar o array ordenado completo. Utilize barriers para sincronizar a conclusão das
ordenações parciais antes de começar a mesclagem.
*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* bubbleSort(int array[], int tamanho);

pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

int main(){

    int tam;
    int n;
    printf("Indique o tamanho do array para ser gerado de forma aleatória: ");
    scanf("%d",&tam);
    int array[tam];
    printf("O array gerado, em ordem é: "\n);
    for (int i = 0; i < tam; i++) {
    array[i] = rand() % 100;
    printf("%d ",array[i]);
    }
    printf("Digite a quantidade de bubblesorts concorrente: ");
    scanf("%d",&n);
    //alocação dos threads
    pthread_t *threads = (pthread_t *)malloc(n * sizeof(pthread_t));
    if(threads == NULL) { printf("Erro na alocacao1\n"); exit(1); };
    int **taskids = (int **)malloc(n * sizeof(int*));
    if(taskids == NULL) { printf("Erro na alocacao2\n"); free(threads); exit(1); };
    //criação dos threads
    int rc;
    
}

void* bubbleSort(int array[], int tamanho) {
    for (int i = 0; i < tamanho - 1; i++) {
        for (int j = 0; j < tamanho - i - 1; j++) {
            if (array[j] > array[j + 1]) {
                swap(&array[j], &array[j + 1]);
            }
        }
    }
}
