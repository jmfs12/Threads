#define _XOPEN_SOURCE 600
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define P 10 // Quantas interações serão feitas
#define incognitas 3 // Número de incógnitas

double A[incognitas][incognitas] = {{2, 1, 3}, {5, 7, 8}, {10, 4, 2}}; // Valores
double b[incognitas] = {11, 13, 15}; // Valores de B
double X[incognitas] = {1, 1, 1}; // Inicial (x^0)
double Xtemp[incognitas]; // Valor temporário após cada somatório
double Xfinal[P][incognitas]; // Vai guardar cada interação
int N; // Threads definidas na main
pthread_barrier_t barrier; // Inicializando barreira

void *jacobi(void *threadid) {
    int id = *(int *)threadid; // Passando o id da thread
    int start = id * (incognitas / N); // onde começará a thread
    int end;// onde terminará, considerando o resto caso a divisão não seja exata entre threads
    if (id == N - 1) {
        end = incognitas;//considerando a ultima thread
    } else {
        end = (id + 1) * (incognitas / N);
    } 
    for (int k = 0; k < P; k++) {
        for (int incognita = start; incognita < end; incognita++) {
            double somatorio = b[incognita]; // Como na fórmula do somatório
            for (int j = 0; j < incognitas; j++) {
                if (incognita != j) {
                    somatorio -= A[incognita][j] * X[j]; // Subtraindo de b[i]
                }
            }
            Xtemp[incognita] = somatorio / A[incognita][incognita]; // (1÷A[i][i]) x somatório
        }
        pthread_barrier_wait(&barrier); // Fazendo todas as threads aguardarem antes de salvar os respectivos resultados 
        if (id == N - 1) { // A "última" thread será responsável por salvar os valores - Ultima entre áspas pois só o escalonador decide que é a ultima
            for (int i = 0; i < incognitas; i++) {
                X[i] = Xtemp[i]; // Salvando em x
                Xfinal[k][i] = Xtemp[i]; // Salvando no array final que servirá para imprimir
            }
        }
        pthread_barrier_wait(&barrier); // Fazendo todas as threads esperarem antes de imprimir
        if (id == N - 1) { // A "última" thread será responsável por imprimir os valores
            printf("\nInteração %d:\n", k + 1);
            for (int i = 0; i < incognitas; i++) {
                printf("X%d = %f\n", i + 1, Xfinal[k][i]); // Imprimindo as incógnitas
            }
        }
        pthread_barrier_wait(&barrier); // Aguarda para retornar ao laço
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    printf("Qual o número de threads?\n");
    scanf("%d", &N);
    pthread_t threads[N];
    int *taskids[N]; // Id da thread
    pthread_barrier_init(&barrier, NULL, N); // Inicializando a barreira
    for (int i = 0; i < N; i++) {
        taskids[i] = (int *)malloc(sizeof(int));
        *taskids[i] = i;
        pthread_create(&threads[i], NULL, jacobi, (void *)taskids[i]);
    }
    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL); // Aguardando todas as threads chegarem para poder encerrar
    }
    pthread_barrier_destroy(&barrier); // Destruindo barreira
    pthread_exit(NULL); // Encerrando threads
}
