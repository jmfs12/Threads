#define _XOPEN_SOURCE 600
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NUM_THREADS 6
#define TAMANHO 12
pthread_barrier_t barrier;

int vet[TAMANHO] = {20,1500,100,54,32,101,5,22,99,0,12,54};
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
                indice = temp;//coletamos o indice para incrementar o vetor de indices "responsável por dizer quem é o próximo número a ser comparado do subarray"
                menor = vet[vetInd[temp]];
                indiceAlt = vetInd[temp];//índice para jogar o tamanho máximo para não replicar os números já colocados
            }
                temp++;
        }
        vet[indiceAlt] = __INT_MAX__;//No lugar do número adcionado vai um max inteiro
        vetFim[i]= menor;//o menor vai no vetor final
        vetInd[indice]++;//incrementa o valor do vetor de indices para que ele compare o próximo número do seu "subarray"
        temp = 0;
        if(vetInd[NUM_THREADS-1] == TAMANHO){//caso o ultimo "subarray" tenha sido posto por completo, para evitar que ele compare com a invasão de memória
            vetInd[NUM_THREADS-1]= (TAMANHO / NUM_THREADS) * (NUM_THREADS-1);//colocamos o vetor de índice para comparar com o primeiro elemento do "subarray"
        }
    }
  pthread_exit(NULL);
}

int main (int argc, char *argv[]){
    pthread_t threads[NUM_THREADS];
    int *taskids[NUM_THREADS];
    pthread_barrier_init(&barrier, NULL, NUM_THREADS+1);//iniciando a barreira para esperar as threads definidas + a main.
    int rc; int t;
    for(t=0; t<NUM_THREADS; t++){
        printf("criando thread %d\n", t);
        taskids[t] = (int *) malloc(sizeof(int));
        *taskids[t] = t;
        rc = pthread_create(&threads[t],NULL,bubble, (void *)taskids[t]);
        if(rc){//Conferindo se as threads estão sendo criadas.
            printf("NÃO FOI POSSÍVEL CRIAR A THREAD %d", t);
            exit(1);
        }
    }
    pthread_barrier_wait(&barrier);//quando todos chegarem incluindo a main, todos passam.
    pthread_t threads2;//inicializando a thread final
    rc = pthread_create(&threads2,NULL,Final, NULL);//criando a thread e passando ela pra função que vai juntar
    if(rc){//conferindo se a thread foi criada.
        printf("NÃO FOI POSSÍVEL CRIAR A THREAD %d",t);
	    exit(1);
	}
    pthread_join(threads2, NULL);//esperando a thread chegar para prosseguir
    for (int i = 0; i < TAMANHO; i++) {
        printf("%d\n", vetFim[i]);
    }
    pthread_barrier_destroy(&barrier);//destruindo barreira
    pthread_exit(NULL);//finalizando prog.
}
