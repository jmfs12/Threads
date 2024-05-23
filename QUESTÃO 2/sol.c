#define _XOPEN_SOURCE 600

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

pthread_barrier_t barrier;

int len;
int parts;
int *vet;
int *vetFim;

void *bubble(void *threadid){
    int tid = *((int *)threadid);
    int temp = 0;
    for(int i = tid * len/parts;i<(tid+1)*len/parts;i++){
      for(int j = tid * len/parts;j<(tid+1)*len/parts;j++){
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
    int vetInd[parts];//Onde ficará armazenado onde começa cada sub-array
    for(int i = 0;i<parts;i++){
        vetInd[i] = len / parts * i;//botando onde cada thread começa
    }
    int indice;//onde será armazenado o índice
    int temp = 0;
    for (int i = 0; i < len; i++) {
        int menor = __INT_MAX__;
        while (temp < parts) {
            //pegar o menor de todos
            if(vet[vetInd[temp]] < menor){
                indice = temp; //coletamos o indice para incrementar o vetor de indices responsavel por dizer quem é o proximo numero a ser comparado do subarray
                menor = vet[vetInd[temp]];
            }
                temp++;
        }
        
        vet[vetInd[indice]] = __INT_MAX__; // no lugar do numero adicionado vai o max inteirp
        vetFim[i]= menor; // o menor vai no vetor final
        vetInd[indice]++; //incrementa o valor do vetor de indices para que ele compare com o próximo número do seu subarray
        temp = 0;
        if(vetInd[parts-1] == len){//caso o ultimo "subarray" tenha sido posto por completo, para evitar que ele compare com a invasão de memória
            vetInd[parts-1]= (len / parts) * (parts-1);//colocamos o vetor de índice para comparar com o primeiro elemento do "subarray"
        }
    }
  pthread_exit(NULL);
}

int main (int argc, char *argv[]){
    printf("Insira a quantidade de partes para dividir o array: ");
    scanf("%d", &parts);

    printf("\nIndique o tamanho do array para ser gerado de forma aleatória: ");
    scanf("%d", &len);

    vet = (int*)malloc(len*sizeof(int));
    vetFim = (int*)malloc(len*sizeof(int));

    printf("\nO array gerado, em ordem é: \n");
    srand(time(NULL));
    for (int i = 0; i < len; i++) {
        vet[i] = rand() % 100;
        printf("%d ",vet[i]);
    }
    
    pthread_t threads[parts];
    int *taskids[parts];

    pthread_barrier_init(&barrier, NULL, parts+1); //iniciando a barreira para esperar as threads definidas + a main
    int rc; int t; int u;
    for(t=0; t<parts; t++){
      printf("\nNo main: criando thread %d", t);
      taskids[t] = (int *) malloc(sizeof(int));
      *taskids[t] = t;
      rc = pthread_create(&threads[t],NULL,bubble, (void *)taskids[t]);
      if(rc){//Conferindo se as threads estão sendo criadas.
        printf("NÃO FOI POSSÍVEL CRIAR A THREAD %d", t);
        exit(1);
      }
    }
    printf("\n");


    pthread_barrier_wait(&barrier); // quanto todos chegarem, incluindo a main, todos passam

    pthread_t threads2;// inicializando a thread final
    rc = pthread_create(&threads2,NULL,Final, NULL); // criando a thread e passando ela pra função que vai juntar
    if(rc){// conferindo se a thread foi criada 
      printf("NÃO FOI POSSÍVEL CRIAR A THREAD FINAL\n");
      exit(1);
    }
    pthread_join(threads2, NULL); // espernado a thread chegar para prosseguir
    for (int i = 0; i < len; i++) {
        printf("%d ", vetFim[i]);
    }
    printf("\n");
    pthread_barrier_destroy(&barrier); // destruindo a barreira
    pthread_exit(NULL); // finalizando programa
}
