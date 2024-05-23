#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define BUFFER_SIZE 100 //definindo um buffer máximo de 100
#define TABLE_SIZE 100

Node* create_elink(Node *nextval);
Node* create_link(Operation it, Node *nextval);
Queue* create_queue();
void enqueue(Queue* q, Operation it);
Operation dequeue(Queue* q);

typedef struct operation{

  int operacao;  //-1 - Operação falha, 0 - Armazenando informações da conta bancária, 1 - saldo, 2 - deposito, 3 - saque
  int conta;
  float saldo;  // caso seja ID 0 esse será o saldo da conta, nos demais ID usaremos como operações bancárias. 
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

typedef struct {
    Node *table[TABLE_SIZE];
} HashTable;

    Queue* buffer;          //buffer do cliente para o banco
    Queue* buffer_output;   //buffer da respsota do banco para o cliente
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t* resposta;
    pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
    pthread_cond_t fill = PTHREAD_COND_INITIALIZER;
    pthread_cond_t* cnd_resposta;

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
    resposta = (pthread_mutex_t *) malloc(N*sizeof(pthread_mutex_t));
    if(resposta==NULL) {printf("Erro na alocação 4.");exit(2);};
    cnd_resposta = (pthread_cond_t *) malloc(N*sizeof(pthread_cond_t));
    if(cnd_resposta==NULL) {printf("Erro na alocação 5.");exit(2);};

    //Criação propriamente dita das threads, utilizamos rc para identificar qualquer possível erro na criação

    for(int i=0;i<N;i++){
        taskids[i] = (int *)malloc(sizeof(int));
        if(taskids[i]==NULL) {printf("Erro na alocação 6.");exit(3);}
        *taskids[i]=i;
        rc = pthread_create(&clientes[i],NULL,fclientes,(void *) taskids[i]);
        if(rc) {printf("Erro na alocação do thread 1.");exit(3);}
        rc = pthread_cond_init(&cnd_resposta[i],NULL);
        if(rc) {printf("Erro na alocacao da condição 1.");exit(3);};
        rc = pthread_mutex_init(&resposta[i],NULL);
        if(rc) {printf("Erro na alocacao do mutex 1.");exit(3);};
    }
    rc = pthread_create(&banco,NULL,fbanco,NULL);
    if(rc) {printf("Erro na alocação do thread 2.");exit(3);}

    
}

Operation put_wait(Operation op,int threadid){
    //para inserir no buffer, bloqueamos que os demais consumidores façam o mesmo para evitar problemas
    pthread_mutex_lock(&mutex);
    if(buffer->size==BUFFER_SIZE){
        printf("Servidor cheio, aguarde um momento.\n");
        while(buffer->size==BUFFER_SIZE){
            pthread_cond_wait(&empty,&mutex);
        }
    }
    enqueue(buffer,op);//inserindo no buffer
    if(buffer->size==1){
        pthread_cond_signal(&fill);
    }
    pthread_mutex_unlock(&mutex);
    //entregamos a solicitacao no buffer para o banco, agora temos que aguardar a resposta
    //Não podemos compartilhar o mutex entre os clientes, já que bloquearia todos ao mesmo tempo
    //logo, vamos ter que utilizar um mutex unico para cada cliente para que ele aguarde a respsota do banco
    //o unlock é feito pelo banco.
    pthread_mutex_lock(&resposta[threadid]);
    return dequeue(buffer_output);
}

void fclientes(int taskid){   //agindo como produtor
    int op=0,conta;
    float valor;
        printf("Olá cliente %d, informe sua conta: ",taskid);
        scanf("%d",&conta);
    while(op!=4){
        printf("(Cliente %d) Digite a operação que deseja realizar na conta %d \n",taskid,conta);
        printf("1. Saldo da conta\n2.Deposito\n3.Saque\n4. Sair\n");
        scanf("%d",&op);
        Operation escolha;
        Operation resposta;
        escolha.conta=conta;
        escolha.operacao=op;
        escolha.threadID=taskid;    //Estamos identificando o thread que solicitou a operação, para que o banco o acorde quando devolver o solicitação
        if(op==1){
            resposta = put_wait(escolha,taskid);
            printf("O saldo da conta %d é de: %d \n",resposta.conta,resposta.saldo);
        }else if(op==2){
            printf("Digite o valor para deposito: ");
            scanf("%d",&valor);
            escolha.saldo=valor;
            resposta = put_wait(escolha,taskid);
            printf("Deposito realizado com sucesso, o novo saldo da conta %d é de: %d\n",resposta.conta,resposta.saldo);
        }else if(op==3){
            printf("Digite o valor para saque: ");
            scanf("%d",&valor);
            escolha.saldo=valor;
            resposta = put_wait(escolha,taskid);
            if(resposta.operacao==-1){  //Se o banco retornou com uma operacao -1, significa que nao ha saldo suficiente
                printf("O saldo da conta é insuficiente para realizar o saque solicitado.\n");
                printf("O saldo é de: %d\n",resposta.saldo);
            }else{
                printf("Saque realizado com sucesso, o novo saldo é de: %d\n",resposta.saldo);
            }
        }else if(op==4){
            printf("Encerrando a sessão.\n");
            break;
        }else printf("Opção inválida, tente novamente.\n");
    }

    pthread_exit(NULL);
}

void fbanco(){  //agindo como consumidor
    Operation processa;
    while(1){
        pthread_mutex_lock(&mutex);
        while(buffer->size==0){
            pthread_cond_wait(&fill,&mutex);
        }
        processa = dequeue(buffer);
        //falta a implementação de hash, procurar a conta e retornar a solicitação processada

        if(buffer->size==BUFFER_SIZE-1) pthread_cond_signal(&empty); //Avisando a algum cliente que tem espaço no servidor
        enqueue(buffer_output,processa);
        pthread_mutex_unlock(&resposta[processa.threadID]); //Desbloqueando o processo, para realizar a coleta da respsota no buffer de saida
        pthread_mutex_unlock(&mutex);
    }
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
Operation* dequeue(Queue* q) {
    if (q->size == 0) 
        return NULL;
    else {
        Operation *it = &(q->front->next->element);
        Node *temp = q->front->next;
        q->front->next = q->front->next->next;
        free(temp);
        if (q->front->next == NULL) 
            q->rear = q->front;
        q->size--;
        return it;
    }
}


// Função de hash simples
int hash(int key) {
    return key % TABLE_SIZE;
}

// Função para inicializar a tabela de hash
void initHashTable(HashTable *hashTable) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable->table[i] = NULL;
    }
}

// Função para inserir uma operação na tabela de hash
void insert(HashTable *hashTable, Operation operation) {
    int index = hash(operation.conta);
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Erro ao alocar memória\n");
        exit(1);
    }
    newNode->element = operation;
    newNode->next = hashTable->table[index];
    hashTable->table[index] = newNode;
}

// Função para procurar uma operação na tabela de hash
Operation *search(HashTable *hashTable, int conta) {
    int index = hash(conta);
    Node *current = hashTable->table[index];
    while (current != NULL) {
        if (current->element.conta == conta) {
            return &current->element;
        }
        current = current->next;
    }
    return NULL; // Se não encontrado
}
