/**
* Copyright(c) 2017 dyf
* use queue to record each task_struct, and list them.
* use TaskNode to collect a task_struct.
*/
#include "myqueue.h"


void initQueue(LinkQueue *queue)
{
    queue->front = queue->rear = (TaskNode*)malloc(sizeof(TaskNode));
    if(NULL == queue->front)
    {
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

    if(NULL == q)
    {
        exit(0);
    }
    q = tmptasknode;
    //printf("add first:%d ",q->minflt);
    q->next = NULL;
    queue->rear->next = q;
    queue->rear = q;
    //printf("front minflt:%d ",queue->front->minflt);
}

void traversal(LinkQueue queue,int sysnum[][11],int psnum)
{
    int i = 0;
    TaskNode* q = queue.front->next;
    TaskNode* freeq;
    char sql_insert[1024];
    while(q != NULL)
    {
        printf("queue %d %s id is: %d\n",i,q->tsname,q->tspid);
        printf("prio:%d parentid:%d grouplead:%d minflt:%ld majflt:%ld ",q->tsprio,q->tsparent,q->tsgroupleader,q->tsminflt,q->tsmajflt);
        printf("utime:%d stime:%d start_time:%lf realstart_time:%lf ",q->tsutime,q->tsstime,q->tsstart_time/60000000000.0,q->tsrealstart_time/60000000000.0);
        printf("total:%d\nUNIX:%d NETLINK:%d TCP:%d UDP:%d TCPv6:%d ",q->tsfdnum,q->socketinfo[0],q->socketinfo[1],q->socketinfo[2],q->socketinfo[3],q->socketinfo[4]);
        printf("[eventfd]:%d inotify:%d [timerfd]:%d [signalfd]:%d [eventpoll]:%d ",q->anon_inodeinfo[0],q->anon_inodeinfo[1],q->anon_inodeinfo[2],q->anon_inodeinfo[3],q->anon_inodeinfo[4]);
        printf("pipe:%d null:%d file:%d\n",q->pipeinfo,q->nullinfo,q->fileinfo);

        int j = 1;
        int totalsyscall = 0;
        printf("%d syscall:",sysnum[i][0]);
        for(; j<11; j++)
        {
            printf(" %d(%d)",sysnum[i][j],j-1);
            totalsyscall+=sysnum[i][j];
            //sysnum[i][j] = 0;
        }
        sprintf(sql_insert,"insert into psinfo(psid,psname,parentid,gleaderid, prio,minflt,majflt,utime,stime,start_time,realstart_time,totalfiles,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,totalsyscall,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation)values('%d','%s','%d','%d', '%d','%ld','%ld','%d','%d','%lf','%lf','%d','%d', '%d','%d','%d','%d','%d','%d', '%d', '%d', '%d', '%d', '%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d');",q->tspid,q->tsname,q->tsparent,q->tsgroupleader,q->tsprio,q->tsminflt,q->tsmajflt,q->tsutime,q->tsstime,q->tsstart_time/60000000000.0,q->tsrealstart_time/60000000000.0,q->tsfdnum,q->socketinfo[0],q->socketinfo[1],q->socketinfo[2],q->socketinfo[3],q->socketinfo[4],q->anon_inodeinfo[0],q->anon_inodeinfo[1],q->anon_inodeinfo[2],q->anon_inodeinfo[3],q->anon_inodeinfo[4],q->pipeinfo,q->fileinfo,totalsyscall,sysnum[i][2],sysnum[i][3],sysnum[i][4],sysnum[i][5],sysnum[i][6],sysnum[i][7],sysnum[i][8],sysnum[i][9],sysnum[i][10]);
        exec_db(sql_insert);
        //printf("\n%s\n",sql_insert);
        freeq = q;
        q = q->next;
        free(freeq);
        i++;
    }
}

void popQueue(LinkQueue *queue)
{
    TaskNode* q = NULL;
    if(!isEmpty(queue))
    {
        q = queue->front->next;
        queue->front->next = q->next;
        if(queue->rear ==q)
        {
            queue->rear = queue->front;
        }
        free(q);
    }
}
