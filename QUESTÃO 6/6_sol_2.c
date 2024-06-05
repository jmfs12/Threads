#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#define BUFFER_SIZE 50 //tamanho maximo do lista_pronta

// implementação de fila de threads
typedef struct node{
    pthread_t element;
    int id;
    struct node* next;
} Node;

typedef struct queue{
    int size;
    Node* front;
    Node* rear;
} Queue;

typedef struct{
    pthread_t thread;
    int id;
} Thread;


Node* create_elink(Node *nextval);
Node* create_link(pthread_t it, Node *nextval, int tid);
Queue* create_queue();
void enqueue(Queue* q, pthread_t it, int tid);
Thread dequeue(Queue* q);

int N;
Queue *lista_pronto;
pthread_mutex_t mutexEsc = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condEsc = PTHREAD_COND_INITIALIZER; 
pthread_mutex_t *mutex;
pthread_cond_t *cond;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;
pthread_mutex_t buf = PTHREAD_MUTEX_INITIALIZER;
pthread_t *threads;
int *ready;
int count=0;
int len;
int **taskids;

void *thread_func(void *threadid){
    int tid = *((int *)threadid);
    pthread_mutex_lock(&mutex[tid]);
    while(ready[tid] == 0){
        pthread_cond_wait(&cond[tid], &mutex[tid]); //aguardando o escalonador liberar a thread
    }
    pthread_mutex_unlock(&mutex[tid]);
    count++;    //quantidade de threads em execução
    printf("Iniciando a execução do thread %d\n", tid);
    int random_microseconds = rand() % 1000000;
    usleep(random_microseconds);        //Aguardando um tempo aleatorio entre 0 e 1 segundo

    //pthread_mutex_lock(&mutex[tid]);
    //ready[tid] = 0;
    //pthread_mutex_unlock(&mutex[tid]);
    //pthread_cond_signal(&cond[tid]);
    count--;
    pthread_mutex_lock(&mutexEsc);  //informando ao escalonador que liberou um núcleo
    pthread_cond_signal(&condEsc);
    pthread_mutex_unlock(&mutexEsc);
    printf("Thread %d finalizada em %d ms\n", tid,random_microseconds);
    pthread_exit(NULL);
}

void *escalonador(void *ar){    //consome da fila e executa as threads

    int i = 0;
    while(1){
        pthread_mutex_lock(&buf);
        while(lista_pronto->size==0){
            pthread_cond_wait(&fill,&buf);    //sem thread para executar
        }
        Thread t = dequeue(lista_pronto);
        if(lista_pronto->size==BUFFER_SIZE-1) pthread_cond_signal(&empty);
        pthread_mutex_unlock(&buf);

        pthread_mutex_lock(&mutexEsc);
        while(count >= N){
            pthread_cond_wait(&condEsc, &mutexEsc);    //nucleos ocupados, aguardando liberar
        }
        pthread_mutex_unlock(&mutexEsc);

        pthread_mutex_lock(&mutex[t.id]);
        ready[t.id] = 1;
        pthread_cond_signal(&cond[t.id]);
        pthread_mutex_unlock(&mutex[t.id]);
        
    }

    pthread_exit(NULL);
}

void* criador(){

    taskids = (int **)malloc(len*sizeof(int*)); //taskid está correto?
    if(taskids == NULL) { printf("Erro de alocação2\n"); exit(1); }

for(int i = 0; i < len; i++){       //A main vai estar agindo como produtor
        taskids[i] = (int *)malloc(sizeof(int));
        *taskids[i] = i;
        pthread_mutex_init(&mutex[i], NULL);
        pthread_cond_init(&cond[i], NULL);
        pthread_create(&threads[i], NULL, thread_func, (void *) taskids[i]);
        pthread_mutex_lock(&buf);
        while(lista_pronto->size==BUFFER_SIZE){
            pthread_cond_wait(&empty, &buf);
        }
        enqueue(lista_pronto, threads[i], i);
        if(lista_pronto->size==1)   pthread_cond_signal(&fill);
        pthread_mutex_unlock(&buf);
        ready[i] = 0;
    }
}


int main(){

    //criação da fila
    lista_pronto = create_queue();
    pthread_t Tescalonador;
    pthread_t Tcriador;

    printf("Digite o número de núcleos: ");
    scanf("%d", &N);
    printf("\n");
    printf("Digite a quantidade de threads: ");
    scanf("%d", &len);

    threads = (pthread_t *)malloc(len*sizeof(pthread_t));
    if(threads == NULL) { printf("Erro de alocação1\n"); exit(1); }
    mutex = (pthread_mutex_t *)malloc(len*sizeof(pthread_mutex_t));
    cond = (pthread_cond_t *)malloc(len*sizeof(pthread_cond_t));
    ready = (int *)malloc(len*sizeof(int));

    pthread_create(&Tescalonador, NULL, escalonador, NULL);
    pthread_create(&Tcriador, NULL, criador, NULL);


    for(int i=0;i<len;i++){
        pthread_join(threads[i],NULL);
    }   //So vamos executar ate que todas as threads finalizem

    printf("Todas as threads finalizaram, fechando a main.\n");

    free(threads);
    for(int i=0;i<len;i++)  free(taskids[i]);
    free(taskids);
    free(mutex);
    free(cond);
    free(ready);
    free(lista_pronto);

    pthread_exit(NULL);
    
    return 0;
}

Node* create_elink(Node *nextval){
    Node* n = (Node*) malloc(sizeof(Node));
    n->next=nextval;
    return n;
}

Node* create_link(pthread_t it, Node *nextval, int tid){
    Node* n = (Node*) malloc(sizeof(Node));
    n->element = it;
    n->id = tid;
    n->next = nextval;
    return n;
}

Queue* create_queue(){
    Queue* q = (Queue*) malloc(sizeof(Queue));
    q->front=q->rear=create_elink(NULL);
    q->size=0;
    return q;
}
void enqueue(Queue* q, pthread_t it, int tid){
    q->rear->next=create_link(it,NULL, tid);
    q->rear=q->rear->next;
    q->size++;
}


Thread dequeue(Queue* q) {
    if (q->size == 0) {
        // fazer retornar alguma coisa
    } else {
        Thread it;
        it.thread = q->front->next->element; // Faz uma cópia dos dados
        it.id = q->front->next->id;
        Node* temp = q->front->next;
        q->front->next = q->front->next->next;
        free(temp);
        if (q->front->next == NULL) 
            q->rear = q->front;
        q->size--;
        return it;
    }
}
