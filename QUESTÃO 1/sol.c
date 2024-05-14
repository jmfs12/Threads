/*
primeiro, inicia a variável mymutex com o inicializador.

na main
lê a palavra procurada e a quantidade de arquivos que serão lidos.
aloca a memória das threads e do id das threads.
cria cada thread e da join

na função read
da lock no mutex, e faz o incremento do id do arquivo no inicio de cada thread, começando por 1, depois da unlock.
cria o nome do arquivo, que será identificado pelo seu ID, ex. "1.txt", "2.txt"
lê cada palavra do arquivo e faz a comparação. Se for igual, da lock no mutex, e atualiza o contador, depois da unlock
e continua o loop. Caso seja diferente, não faz nada.

dúvida: *não sei se precisa dar free nos ponteiros da main, acho que o pthread_exit já faz isso, porém eu dei close nos arquivos
    e dei free na verificação das alocações.
resp: rodei com o valgrind, e faltava dar free em alguns ponteiros, então dei free em threads e em taskids
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int count = 0;
int fileID = 0;
char word[50]; 
int x;
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

void *read(void *threadid);

int main(){
    // leitura da palavra e do número de arquivos
    printf("Digite a palavra: ");
    scanf(" %s", word);
    printf("\nDigite o número de arquivos: ");
    scanf("%d", &x);
    // alocacao da quantidade de threads para ler os arquivos
    pthread_t *threads = (pthread_t *)malloc(x * sizeof(pthread_t));
    if(threads == NULL) { printf("Erro na alocacao1\n"); exit(1); };
    int **taskids = (int **)malloc(x * sizeof(int*));
    if(taskids == NULL) { printf("Erro na alocacao2\n"); free(threads); exit(1); };

    int rc = 0;
    // criação das threads
    for(int i = 0; i < x; i++){
        taskids[i] = (int *)malloc(sizeof(int));
        if(taskids[i] == NULL) { printf("Erro na alocacao3"); free(threads); free(taskids); exit(1); }
        *taskids[i] = i;
        rc = pthread_create(&threads[i], NULL, read, (void *) taskids[i]);
        if (rc){         
            printf("ERRO NA CRIACAO DA THREAD");       
            free(threads);
            for(int j = 0; j < i; j++)
                free(taskids[j]);
            free(taskids);  
            exit(-1);      
        }   
    }
    
    //controle de entrada das threads   
    for(int u = 0; u < x; u++){
        pthread_join(threads[u], NULL);
    }

    printf("\nA palavra apareceu %d vezes.\n", count);

    free(threads);
    for(int i = 0; i < x; i++)
        free(taskids[i]);
    free(taskids);

    pthread_exit(NULL);
}

void *read(void *threadid){
    // incremento do ID do arquivo, usando mutex, para que as threads acessem diferentes arquivos.
    pthread_mutex_lock(&mymutex); 
    fileID++; 
    pthread_mutex_unlock(&mymutex);
    // ajuste do nome do arquivo
    FILE *arq;
    char filename[20];
    sprintf(filename, "%d.txt", fileID);
 
    arq = fopen(filename, "r");
    if(arq == NULL) { printf("Falha ao abrir o arquivo"); exit(1); }

    char aux[50];
    // leitura de cada palavra e incremento
    while(fscanf(arq, "%s", aux) == 1){
        
        if(strcmp(word, aux) == 0){
            pthread_mutex_lock(&mymutex);
            count++;
            pthread_mutex_unlock(&mymutex);
        }
    }

    fclose(arq);
    pthread_exit(NULL);
}
