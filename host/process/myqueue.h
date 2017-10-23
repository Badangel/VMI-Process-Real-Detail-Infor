#ifndef MYQUEUE_H_INCLUDED
#define MYQUEUE_H_INCLUDED
#include <taskinfo.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct TaskNode{
    uint32_t tspid;
    unsigned int tsfdnum;
    //socket in UNIX,NETLINK,TCP,UDP,TCPv6,other
    FdInfo *tsfdinfo;
    int tssocketinfo[6];
    //anon_inode in eventfd,inotify,timerfd,signalfd,other
    int tsanon_inodeinfo[5];
    int tspipeinfo;
    int tsnullinfo;
    int tsnuminfo;

    uint32_t tsparent;
    uint32_t tsgroupleader;
    int tsprio;
    unsigned int tsutime;
    unsigned int tsstime;
    uint64_t tsstart_time;
    uint64_t tsrealstart_time;
    unsigned long tsminflt;
    unsigned long tsmajflt;
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

bool isEmpty(LinkQueue *queue)
{
    return queue->rear == queue->front ? true : false;
}

void pushQueue(LinkQueue *queue, TaskNode* tmptasknode)
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

void traversal(LinkQueue *queue)
{
    int i = 1;
    TaskNode* q = queue->front->next;
    while(q != NULL){
        printf("queue %d id is: %d\n",i,q->tspid);
        printf("prio:%d parentid:%d utimes:%d minflt:%d\n",q->tsprio,q->tsparent,q->tsutime,q->tsminflt);
        printf("total:%d||UNIX:%d NETLINK:%d TCP:%d UDP:%d TCPv6:%d",q->tsfdinfo->numfd,q->tsfdinfo->socketinfo[0],q->tsfdinfo->socketinfo[1],q->tsfdinfo->socketinfo[2],q->tsfdinfo->socketinfo[3],q->tsfdinfo->socketinfo[4],q->tsfdinfo->socketinfo[5]);
        q = q->next;
        i++;
    }
}

void popQueue(LinkQueue *queue)
{
    TaskNode* q = NULL;
    if(!isEmpty(queue)){
        q = queue->front->next;
        queue->front->next = q->next;
        if(queue->rear ==q){
            queue->rear = queue->front;
        }
        free(q);
    }
}

#endif // MYQUEUE_H_INCLUDED
