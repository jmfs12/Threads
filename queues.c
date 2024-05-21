#include "linkedlist.h"
#include "queues.h"
#include <stdio.h>
#include <stdlib.h>

Queue create_queue(){
    Queue q;
    q.front = q.rear = (Link *)malloc(sizeof(Link));
    q.front->next = q.rear->next = NULL;
    q.size = 0;
    return q;
}
void clear(Queue *q){

}
void enqueue(Queue *q, int item){
    q->rear->next = create_link(item, NULL);
    q->rear = q->rear->next;
    q->size++;
}
int dequeue(Queue *q){
    if(q->size == 0)
        return -1;
    else{
        int item = q->front->next->element;
        q->front->next = q->front->next->next;
        if(q->front->next == NULL)
            q->rear = q->front;
        q->size--;
        return item;
    }
}
int frontValue(Queue q){
    if(q.front != NULL)
        return q.front->element;
}
int length(Queue q){
    if(q.size > 0)
        return q.size;
}