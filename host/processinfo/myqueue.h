/**
* Copyright(c) 2017 dyf
* use queue to record each task_struct, and list them.
* use TaskNode to collect a task_struct.
*/

#ifndef MYQUEUE_H_INCLUDED
#define MYQUEUE_H_INCLUDED

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <mysql/mysql.h>

typedef struct TaskNode{
    uint32_t tspid;
    unsigned int tsfdnum;
    //socket in UNIX,NETLINK,TCP,UDP,TCPv6,other
    //FdInfo tsfdinfo;
    int socketinfo[6];
    //anon_inode in [eventfd],inotify,[timerfd],[signalfd],[eventpoll],other
    int anon_inodeinfo[6];
    int pipeinfo;
    int nullinfo;
    int numinfo;
    int fileinfo;

    uint32_t tsparent;
    uint32_t tsgroupleader;
    int tsprio;
    unsigned int tsutime;
    unsigned int tsstime;
    uint64_t tsstart_time;
    uint64_t tsrealstart_time;
    unsigned long tsminflt;
    unsigned long tsmajflt;

    int syscallnum[11];

    char tsname[255];
    int state;
    struct TaskNode* next;

}TaskNode;

typedef struct{
    TaskNode* front;
    TaskNode* rear;
}LinkQueue;


void initQueue(LinkQueue *queue);

bool isEmpty(LinkQueue *queue);

void pushQueue(LinkQueue *queue, TaskNode* tmptasknode);

int compare2TaskNode(TaskNode* p,TaskNode* q);

int findSamePointinQueue(TaskNode* q,LinkQueue* pre_queue);

void popQueue(LinkQueue *queue);

void freeQueue(LinkQueue* queue);

void traversal(MYSQL *mysql,LinkQueue* queue,LinkQueue* pre_queue);


/*
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

void traversal(LinkQueue queue)
{
    int i = 1;
    TaskNode* q = queue.front->next;
    while(q != NULL){
        printf("queue %d %s id is: %d\n",i,q->tsname,q->tspid);
        printf("prio:%d parentid:%d grouplead:%d minflt:%ld majflt:%ld\n",q->tsprio,q->tsparent,q->tsgroupleader,q->tsminflt,q->tsmajflt);
        printf("utime:%d stime:%d start_time:%lf realstart_time:%lf\n",q->tsutime,q->tsstime,q->tsstart_time/60000000000.0,q->tsrealstart_time/60000000000.0);
        printf("total:%d\nUNIX:%d NETLINK:%d TCP:%d UDP:%d TCPv6:%d\n",q->tsfdnum,q->socketinfo[0],q->socketinfo[1],q->socketinfo[2],q->socketinfo[3],q->socketinfo[4]);
        printf("[eventfd]:%d inotify:%d [timerfd]:%d [signalfd]:%d [eventpoll]:%d\n",q->anon_inodeinfo[0],q->anon_inodeinfo[1],q->anon_inodeinfo[2],q->anon_inodeinfo[3],q->anon_inodeinfo[4]);
        printf("pipe:%d null:%d file:%d \n\n",q->pipeinfo,q->nullinfo,q->fileinfo);

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
*/
#endif // MYQUEUE_H_INCLUDED
