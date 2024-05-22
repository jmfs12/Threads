#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct operation{

  int ID;  // 0 - conta bancária, 1 - deposito, 2 - saque, 3 - consulta de saldo
  int agencia;
  int conta;
  int saldo;  // caso seja ID 0 esse será o saldo da conta, nos demais ID usaremos como operações bancárias. 

}Operation;

typedef struct node{
    Operation* element;
    struct node* next;
} Node;

typedef struct queue{
    int size;
    Node* front;
    Node* rear;
} Queue;

int main(){

}

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
Operation* dequeue(Queue* q){
    if(q->size==0) return 0;
    else{
        Operation* it=q->front->next->element;
        q->front->next=q->front->next->next;
        if(q->front->next==NULL) q->rear=q->front;
        q->size--;
        return it;
    }
}
