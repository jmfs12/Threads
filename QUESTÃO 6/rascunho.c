#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

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
int *ready;
int flag;

void *thread_func(void *threadid){
    int tid = *((int *)threadid);
    pthread_mutex_lock(&mutex[tid]);
    while(ready[tid] == 0){
        pthread_cond_wait(&cond[tid], &mutex[tid]);
    }
    pthread_mutex_unlock(&mutex[tid]);
    // codigo da thread
    printf("Thread %d\n", tid);
    sleep(1);

    pthread_mutex_lock(&mutex[tid]);
    ready[tid] = 0;
    pthread_mutex_unlock(&mutex[tid]);
    pthread_cond_signal(&cond[tid]);

    pthread_exit(NULL);
}

void *escalonador(void *ar){

    pthread_mutex_lock(&mutexEsc);
    while(!flag){
        pthread_cond_wait(&condEsc, &mutexEsc);
    }
    pthread_mutex_unlock(&mutexEsc);

    int i = 0;
    while(lista_pronto->size > 0){
        if(lista_pronto->size == 0) break;

        Thread t = dequeue(lista_pronto);
        pthread_mutex_lock(&mutex[t.id]);
        ready[t.id] = 1;
        pthread_cond_signal(&cond[t.id]);
        pthread_mutex_unlock(&mutex[t.id]);
        
        if(i >= N-1){
            pthread_join(t.thread, NULL);
            i = 0;
        } else {
            i++;
        }
    }
    pthread_mutex_lock(&mutexEsc);
    flag = 0;
    pthread_cond_signal(&condEsc);
    pthread_mutex_unlock(&mutexEsc);

    pthread_exit(NULL);
}

int main(){

    //criação da fila
    lista_pronto = create_queue();
    flag = 0;
    pthread_t Tescalonador;
    pthread_create(&Tescalonador, NULL, escalonador, NULL);

    int len;
    printf("Digite o número de núcleos: ");
    scanf("%d", &N);
    printf("\n");
    printf("Digite a quantidade de threads: ");
    scanf("%d", &len);

    pthread_t *threads = (pthread_t *)malloc(len*sizeof(pthread_t));
    if(threads == NULL) { printf("Erro de alocação1\n"); exit(1); }
    int **taskids = (int **)malloc(len*sizeof(int*));
    if(taskids == NULL) { printf("Erro de alocação2\n"); exit(1); }
    mutex = (pthread_mutex_t *)malloc(len*sizeof(pthread_mutex_t));
    cond = (pthread_cond_t *)malloc(len*sizeof(pthread_cond_t));
    ready = (int *)malloc(len*sizeof(int));


    for(int i = 0; i < len; i++){
        taskids[i] = (int *)malloc(sizeof(int));
        *taskids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, (void *) taskids[i]);
        enqueue(lista_pronto, threads[i], i);
        pthread_mutex_init(&mutex[i], NULL);
        pthread_cond_init(&cond[i], NULL);
        ready[i] = 0;
    }

    pthread_mutex_lock(&mutexEsc);
    flag = 1;
    pthread_cond_signal(&condEsc);
    pthread_mutex_unlock(&mutexEsc);
    

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
