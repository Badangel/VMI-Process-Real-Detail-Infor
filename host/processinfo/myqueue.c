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

int compare2TaskNode(TaskNode* p,TaskNode* q){
    if(p->pipeinfo != q->pipeinfo||p->nullinfo != q->nullinfo||p->numinfo != q->numinfo||p->fileinfo != q->fileinfo){
        return 0;
    }
    if(p->tsminflt != q->tsminflt||p->tsmajflt != q->tsmajflt||p->tsutime != q->tsutime||p->tsstime != q->tsstime){
        return 0;
    }
    if(p->tsparent != q->tsparent||p->tsgroupleader != q->tsgroupleader||p->tsprio != q->tsprio){
        return 0;
    }
    int i = 0;
    for(; i<6; i++){
        if(p->socketinfo[i] != q->socketinfo[i]||p->anon_inodeinfo[i] != q->anon_inodeinfo[i]||p->syscallnum[i] != q->syscallnum[i]){
            return 0;
        }
    }
    for(; i<11; i++){
        if(p->syscallnum[i] != q->syscallnum[i]){
            return 0;
        }
    }
    return 1;
}

/* find the value same point in a queue, if find it return 1, else return 0 */
int findSamePointinQueue(TaskNode* p,LinkQueue *pre_queue){
    TaskNode* q = pre_queue->front->next;
    while(q != NULL)
    {
        if(p->tspid == q->tspid && strcmp(p->tsname,q->tsname) == 0){
            return compare2TaskNode(p,q);
        }
        if(p->tspid < q->tspid){
            return 0;
        }
        q = q->next;
    }
    return 0;
}

void traversal(MYSQL *mysql,LinkQueue *queue,LinkQueue *pre_queue)
{
    TaskNode* q = queue->front->next;
    TaskNode* freeq;
    char sql_insert[1024];
    int insertsqlnum = 0;
    while(q != NULL)
    {
        /*
        printf("queue %d %s id is: %d\n",i,q->tsname,q->tspid);
        printf("prio:%d parentid:%d grouplead:%d minflt:%ld majflt:%ld ",q->tsprio,q->tsparent,q->tsgroupleader,q->tsminflt,q->tsmajflt);
        printf("utime:%d stime:%d start_time:%lf realstart_time:%lf ",q->tsutime,q->tsstime,q->tsstart_time/60000000000.0,q->tsrealstart_time/60000000000.0);
        printf("total:%d\nUNIX:%d NETLINK:%d TCP:%d UDP:%d TCPv6:%d ",q->tsfdnum,q->socketinfo[0],q->socketinfo[1],q->socketinfo[2],q->socketinfo[3],q->socketinfo[4]);
        printf("[eventfd]:%d inotify:%d [timerfd]:%d [signalfd]:%d [eventpoll]:%d ",q->anon_inodeinfo[0],q->anon_inodeinfo[1],q->anon_inodeinfo[2],q->anon_inodeinfo[3],q->anon_inodeinfo[4]);
        printf("pipe:%d null:%d file:%d\n",q->pipeinfo,q->nullinfo,q->fileinfo);
        */
        ///if(q->tsparent >= 0)///can't insert all ps,180+ ps need 6s,but parentid = 2 have no mean
        ///{
        if(q->state != 2&&!findSamePointinQueue(q,pre_queue)){
            /*classfity 0 syscall not insert*/
            sprintf(sql_insert,"insert into psinfo(psid,psname,parentid,gleaderid, prio,minflt,majflt,utime,stime,start_time,realstart_time,totalfiles,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,totalsyscall,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation,state)values('%d','%s','%d','%d', '%d','%ld','%ld','%d','%d','%lf','%lf','%d','%d', '%d','%d','%d','%d','%d','%d', '%d', '%d', '%d', '%d', '%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d');",q->tspid,q->tsname,q->tsparent,q->tsgroupleader,q->tsprio,q->tsminflt,q->tsmajflt,q->tsutime,q->tsstime,q->tsstart_time/60000000000.0,q->tsrealstart_time/60000000000.0,q->tsfdnum,q->socketinfo[0],q->socketinfo[1],q->socketinfo[2],q->socketinfo[3],q->socketinfo[4],q->anon_inodeinfo[0],q->anon_inodeinfo[1],q->anon_inodeinfo[2],q->anon_inodeinfo[3],q->anon_inodeinfo[4],q->pipeinfo,q->fileinfo,q->syscallnum[10],q->syscallnum[1],q->syscallnum[2],q->syscallnum[3],q->syscallnum[4],q->syscallnum[5],q->syscallnum[6],q->syscallnum[7],q->syscallnum[8],q->syscallnum[9],q->state);

            exec_db(mysql,sql_insert);
            insertsqlnum++;
        }
        ///}
        ///printf("\n%s\n",sql_insert);
        ///freeq = q;
        q = q->next;
        
        ///free(freeq);
    }
   /// myfreeQueue(pre_queue);
    pre_queue->front->next = queue->front->next;
    printf("insert sql num: %d\n",insertsqlnum);
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
