#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define BUFFER_SIZE 100 //definindo um buffer máximo de 100

Node* create_elink(Node *nextval);
Node* create_link(Operation it, Node *nextval);
Queue* create_queue();
void enqueue(Queue* q, Operation it);
Operation dequeue(Queue* q);

typedef struct operation{

  int operacao;  // 0 - Armazenando informações da conta bancária, 1 - saldo, 2 - deposito, 3 - saque
  int conta;
  int saldo;  // caso seja ID 0 esse será o saldo da conta, nos demais ID usaremos como operações bancárias. 
  int threadID; //identificação do thread que a solicitou

}Operation;

typedef struct node{
    Operation element;
    struct node* next;
} Node;

typedef struct queue{
    int size;
    Node* front;
    Node* rear;
} Queue;

    Queue* buffer;          //buffer do cliente para o banco
    Queue* buffer_output;   //buffer da respsota do banco para o cliente
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
    pthread_cond_t full = PTHREAD_COND_INITIALIZER;

int main(){
    int N,rc;

    //Os clientes atuarão como produtores e o banco como consumidor, enquanto utilizaremos uma fila como buffer
    buffer = create_queue();
    buffer_output = create_queue();

    printf("Digite a quantidade de clientes: ");
    scanf("%d",&N);
    //Alocação comum da thread
    pthread_t* clientes = (pthread_t*) malloc(N*sizeof(pthread_t));
    if(clientes==NULL) {printf("Erro na alocação 1.");exit(2);}
    int **taskids = (int **)malloc((N) * sizeof(int*));
    if(taskids==NULL) {printf("Erro na alocação 2.");exit(2);}
    pthread_t* banco = (pthread_t*) malloc(sizeof(pthread_t));
    if(banco==NULL) {printf("Erro na alocação 3.");exit(2);}

    //Criação propriamente dita das threads, utilizamos rc para identificar qualquer possível erro na criação
    for(int i=0;i<N;i++){
        taskids[i] = (int *)malloc(sizeof(int));
        if(taskids[i]==NULL) {printf("Erro na alocação 4.");exit(2);}
        *taskids[i]=i;
        rc = pthread_create(&clientes[i],NULL,fclientes,(void *) taskids[i]);
        if(rc) {printf("Erro na alocação do thread 1.");exit(3);}
    }
    rc = pthread_create(&banco,NULL,fbanco,NULL);
    if(rc) {printf("Erro na alocação do thread 2.");exit(3);}

    
}

void fclientes(int taskid){   //agindo como produtor
    int op=0;
    while(op!=4){
        printf("Olá cliente %d, digite a operação que deseja realizar. \n",taskid+1);
        printf("1. Saldo da conta\n2.Deposito\n3.Saque\n4. Sair\n");
        scanf("%d",&op);
        if(op==1)
    }

}

void fbanco(){  //agindo como consumidor

}


//Implementação de fila
Node* create_elink(Node *nextval){
    Node* n = (Node*) malloc(sizeof(Node));
    n->next=nextval;
    return n;
}

Node* create_link(Operation it, Node *nextval){
    Node* n = (Node*) malloc(sizeof(Node));
    n->element = it;
    n->next = nextval;
    return n;
}

Queue* create_queue(){
    Queue* q = (Queue*) malloc(sizeof(Queue));
    q->front=q->rear=create_elink(NULL);
    q->size=0;
    return q;
}
void enqueue(Queue* q, Operation it){
    q->rear->next=create_link(it,NULL);
    q->rear=q->rear->next;
    q->size++;
}
Operation dequeue(Queue* q){
    if(q->size==0) return 0;
    else{
        Operation it=q->front->next->element;
        q->front->next=q->front->next->next;
        if(q->front->next==NULL) q->rear=q->front;
        q->size--;
        return it;
    }
}
