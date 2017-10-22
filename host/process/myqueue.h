#ifndef MYQUEUE_H_INCLUDED
#define MYQUEUE_H_INCLUDED
#include <taskinfo.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct TaskNode{
    struct fdinfo *fsfdinfo;
    int tsparent;
    int tsgroupleader;
    int tsprio;
    unsigned int tsutime;
    unsigned int tsstime;
    uint64_t tsstart_time;
    uint64_t tsrealstart_time;
    unsigned long minflt;
    unsigned long majflt;
    struct TaskNode* next;

}TaskNode;

typedef struct{
    TaskNode* front;
    TaskNode* rear;

}LinkQueue;

void initQueue(LinkQueue *queue)
{
    queue->front = queue->rear = (TaskNode*)malloc(sizeof(TaskNode));
    if(NULL == queue->front){
        exit(0);
    }
    queue->front->next = NULL;
}

bool isEmpty(LinkQueue queue)
{
    return queue.rear == queue.front ? true : false;
}

void insertQueue(LinkQueue *queue, TaskNode* tmptasknode)
{
    TaskNode* q = (TaskNode*)malloc(sizeof(TaskNode));

    if(NULL == q){
        exit(0);
    }
    q = tmptasknode;
    //printf("add first:%d ",q->minflt);
    q->next = NULL;
    queue->rear->next = q;
    queue->rear = q;
    //printf("front minflt:%d ",queue->front->minflt);
}

void traversal(LinkQueue queue)
{
    int i = 1;
    TaskNode* q = queue.front->next;
    while(q != NULL){
        printf(" queue %d is: %d\n",i,q->minflt);
        q = q->next;
        i++;
    }
}

void deleteQueue(LinkQueue *queue)
{
    TaskNode* q = NULL;
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
