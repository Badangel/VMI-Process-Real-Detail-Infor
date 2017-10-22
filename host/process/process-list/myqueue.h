#ifndef MYQUEUE_H_INCLUDED
#define MYQUEUE_H_INCLUDED
#include <taskinfo.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct TaskNode{
    TaskInfo* taskinfo;
    struct TaskNode* next;

}TaskNode,Queue;

typedef struct{
    Queue* front;
    Queue* rear;

}LinkQueue;

void initQueue(LinkQueue *queue)
{
    queue->front = queue->rear = (Queue)malloc(sizeof(TaskNode));
    if(NULL == queue->front){
        exit(0);
    }
    queue->front->next = NULL;
}

bool isEmpty(LinkQueue queue)
{
    return queue.rear == queue.front ? true : false;
}

void insertQueue(LinkQueue *queue, TaskInfo* tmptaskinfo)
{
    Queue q = (Queue)malloc(sizeof(TaskNode));

    if(NULL == q){
        exit(0);
    }
    q->taskinfo = tmptaskinfo;
    q->next = NULL;
    queue->rear->next = q;
    queue->rear = q;
}

void deleteQueue(LinkQueue *queue)
{
    Queue q = NULL;
    if(!isEmpty(*queue)){
        q = queue->front->next;
        queue->front->next = q->next;
        if(queue->rear ==q){
            queue->rear = queue->front;
        }
        free(q);
    }
}

#endif // MYQUEUE_H_INCLUDED
