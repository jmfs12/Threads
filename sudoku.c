#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 12
#define SIZE 9

pthread_mutex_t mutex;
int matriz[SIZE][SIZE];
int valido = 1;

// função p thread p verificar uma linha
void* verificalinha(void* arg) {
    int linha = *(int*)arg;
    int usados[SIZE + 1] = {0}; // array p marcar os números já utilizados

    for (int j = 0; j < SIZE; j++) {
        int num = matriz[linha][j];
        if (num < 1 || num > 9 || usados[num]) {
            pthread_mutex_lock(&mutex);
            valido = 0;
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }
        usados[num] = 1;
    }
    pthread_exit(NULL);
}

// função p thread p verificar uma coluna
void* verificacoluna(void* arg) {
    int coluna = *(int*)arg;
    int usados[SIZE + 1] = {0}; // array p marcar os números já utiilizados

    for (int i = 0; i < SIZE; i++) {
        int num = matriz[i][coluna];
        if (num < 1 || num > 9 || usados[num]) {
            pthread_mutex_lock(&mutex);
            valido = 0;
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }
        usados[num] = 1;
    }
    pthread_exit(NULL);
}

// função p 9 threads restantes p verificar subgrades
void* verificasubgrade(void* arg) {
    int index = *(int*)arg;
    int linha_inicial = (index / 3) * 3;
    int coluna_inicial = (index % 3) * 3;
    int usados[SIZE + 1] = {0}; // array p marcar os números já utiilizados
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int num = matriz[linha_inicial + i][coluna_inicial + j];
            if (num < 1 || num > 9 || usados[num]) {
                pthread_mutex_lock(&mutex);
                valido = 0;
                pthread_mutex_unlock(&mutex);
                pthread_exit(NULL);
            }
            usados[num] = 1;
        }
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];

    pthread_mutex_init(&mutex, NULL);

    // preencher a matriz com uma solução de Sudoku para teste
    // é p substituir essa matriz por uma matriz de Sudoku para teste
    int sudoku[SIZE][SIZE] = {
        {5, 3, 4, 6, 7, 8, 9, 1, 2},
        {6, 7, 2, 1, 9, 5, 3, 4, 8},
        {1, 9, 8, 3, 4, 2, 5, 6, 7},
        {8, 5, 9, 7, 6, 1, 4, 2, 3},
        {4, 2, 6, 8, 5, 3, 7, 9, 1},
        {7, 1, 3, 9, 2, 4, 8, 5, 6},
        {9, 6, 1, 5, 3, 7, 2, 8, 4},
        {2, 8, 7, 4, 1, 9, 6, 3, 5},
        {3, 4, 5, 2, 8, 6, 1, 7, 9}
    };
    // copia a matriz p matriz global
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            matriz[i][j] = sudoku[i][j];
        }
    }
    // cria threads p verificar as linhas
    for (int i = 0; i < SIZE; i++) {
        int *id =(int*)malloc (sizeof(int));
        id[0] = i;
        pthread_create(&threads[i], NULL, verificalinha, &id[0]);
    }
    // cria threads p verificar as colunas
    for (int i = 0; i < SIZE; i++) {
        int *id =(int*)malloc (sizeof(int));
        id[0] = i;
        pthread_create(&threads[SIZE + i], NULL, verificacoluna, &id[0]);
    }
     // cria threads p verificar as subgrades
    for (int i = 0; i < SIZE; i++) {
        int *id =(int*)malloc (sizeof(int));
        id[0] = i;
        pthread_create(&threads[2 * SIZE + i], NULL, verificasubgrade, &id[0]);
    }
     // aguarda todas as threads terminarem
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    if (valido) {
        printf("Sudoku apresenta uma solução válida\n");
    } else {
        printf("Sudoku apresenta uma solução inválida\n");
    }
    // destroi o mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}
