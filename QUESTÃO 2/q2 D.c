#define _XOPEN_SOURCE 600
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NUM_THREADS 2
#define TAMANHO 10
pthread_barrier_t barrier;

int vet[TAMANHO] = {20,15,23,54,32,1,5,22,99,0};
int vetFim[TAMANHO];
void *bubble(void *threadid){
  int *tid = (int *)threadid;
  int temp = 0;
  for(int i = tid[0] * TAMANHO/NUM_THREADS;i<(tid[0]+1)*TAMANHO/NUM_THREADS;i++){
    for(int j = tid[0] * TAMANHO/NUM_THREADS;j<(tid[0]+1)*TAMANHO/NUM_THREADS;j++){
      if(vet[i]<vet[j]){
        temp = vet[i];
        vet[i] = vet[j];
        vet[j] = temp;
      }
    }
  }
  pthread_barrier_wait(&barrier);
  pthread_exit(NULL);
}

void *Final(void *threadid) {
    //int *tid = (int *)threadid;
    int vetInd[NUM_THREADS];//Onde ficará armazenado onde começa cada sub-array
    for(int i = 0;i<NUM_THREADS;i++){
        vetInd[i] = TAMANHO / NUM_THREADS * i;//botando onde cada thread começa
    }
    int indice;//onde será armazenado o índice
    int temp = 0;
    int indiceAlt;
    for (int i = 0; i < TAMANHO; i++) {
        int menor = __INT_MAX__;
        while (temp < NUM_THREADS) {
            //pegar o menor de todos
            if(vet[vetInd[temp]] < menor){
                indice = temp;
                menor = vet[vetInd[temp]];
                indiceAlt = vetInd[temp];//índice para jogar o tamanho máximo para não replicar os números já colocados
            }
                temp++;
        }
        //incrementar o vetInd na pos da thread do menor
        vet[indiceAlt] = __INT_MAX__;
        vetFim[i]= menor;
        vetInd[indice]++;
        temp = 0;
    }
  pthread_exit(NULL);
}

int main (int argc, char *argv[]){
  pthread_t threads[NUM_THREADS];
  int *taskids[NUM_THREADS];
  pthread_barrier_init(&barrier, NULL, NUM_THREADS+1);
   int rc; int t; int u;
  for(t=0; t<NUM_THREADS; t++){
    printf("No main: criando thread %d\n", t);
    taskids[t] = (int *) malloc(sizeof(int));
    *taskids[t] = t;
	pthread_create(&threads[t],NULL,bubble, (void *)taskids[t]);
  }
  pthread_barrier_wait(&barrier);
  pthread_t threads2;
  pthread_create(&threads2,NULL,Final, NULL);
  pthread_join(threads2, NULL);
  for (int i = 0; i < 10; i++) {
      printf("%d\n", vetFim[i]);
  }
  pthread_barrier_destroy(&barrier);
  pthread_exit(NULL);
}
