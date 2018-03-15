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
    if(p->tsinc_minflt != q->tsinc_minflt||p->tsinc_majflt != q->tsinc_majflt||p->tsinc_utime != q->tsinc_utime||p->tsinc_stime != q->tsinc_stime){
        return 0;
    }
    if(p->tsparent != q->tsparent||p->tsgroupleader != q->tsgroupleader||p->tsprio != q->tsprio){
        return 0;
    }
    if(p->tsinc_rchar != q->tsinc_rchar||p->tsinc_wchar != q->tsinc_wchar||p->tsinc_syscr != q->tsinc_syscr||p->tsinc_syscw != q->tsinc_syscw||p->tsinc_read_bytes != q->tsinc_read_bytes||p->tsinc_write_bytes != q->tsinc_write_bytes||p->tsinc_cancelled_write_bytes != q->tsinc_cancelled_write_bytes){
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
            p->tsinc_majflt = p->tsmajflt - q->tsmajflt;
            p->tsinc_minflt = p->tsminflt - q->tsminflt;
            p->tsinc_stime = p->tsstime - q->tsstime;
            p->tsinc_utime = p->tsutime - q->tsutime;
            p->tsinc_rchar = p->tsrchar - q->tsrchar;
            p->tsinc_wchar = p->tswchar - q->tswchar;
            p->tsinc_syscr = p->tssyscr - q->tssyscr;
            p->tsinc_syscw = p->tssyscw - q->tssyscw;
            p->tsinc_read_bytes = p->tsread_bytes - q->tsread_bytes;
            p->tsinc_write_bytes = p->tswrite_bytes - q->tswrite_bytes;
            p->tsinc_cancelled_write_bytes = p->tscancelled_write_bytes - q->tscancelled_write_bytes;
            return compare2TaskNode(p,q);
        }
        if(p->tspid < q->tspid){
            return 0;
        }
        q = q->next;
    }
    return 0;
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

void freeQueue(LinkQueue* queue){
    TaskNode* q = NULL;
    while(!isEmpty(queue))
    {
        q = queue->front->next;
        queue->front->next = q->next;
        if(queue->rear ==q)
        {
            queue->rear = queue->front;
        }
        freeMyList(q->socket_list);
        freeMyList(q->file_list);
        free(q);
    }
}

void traversal(VmiInfo* vmivm,MYSQL *mysql,LinkQueue *queue,LinkQueue *pre_queue,int frenum,int table)
{
    TaskNode* q = queue->front->next;
    TaskNode* freeq;
    char sql_insert[1024];
    int insertsqlnum = 0;
    bool prequeue_state = isEmpty(pre_queue);
    while(q != NULL&&!prequeue_state)
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
        if(q->state != 3&&(!findSamePointinQueue(q,pre_queue)||frenum)){
            /*classfity 0 syscall not insert*/
            if(table==2){
                sprintf(sql_insert,"insert into nowpsinfo(domname,psid,psname,parentid,layer,gleaderid, prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,total_vm,stack_vm,inc_rchar,inc_wchar,inc_syscr,inc_syscw,inc_read_bytes,inc_write_bytes,inc_cancelled_write_bytes,start_time,realstart_time,totalfiles,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,totalsyscall,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation,state)values('%s','%d','%s','%d','%d','%d', '%d','%ld','%ld','%d','%d','%d','%d','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%lf','%lf','%d','%d', '%d','%d','%d','%d','%d','%d', '%d', '%d', '%d', '%d', '%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d');",vmivm->vmname,q->tspid,q->tsname,q->tsparent,q->tslayer,q->tsgroupleader,q->tsprio,q->tsinc_minflt,q->tsinc_majflt,q->tsinc_utime,q->tsinc_stime,q->tsmm_users,q->tsmm_count,q->tstotal_vm,q->tsstack_vm,q->tsinc_rchar,q->tsinc_wchar,q->tsinc_syscr,q->tsinc_syscw,q->tsinc_read_bytes,q->tsinc_write_bytes,q->tsinc_cancelled_write_bytes,q->tsstart_time/60000000000.0,q->tsrealstart_time/60000000000.0,q->tsfdnum,q->socketinfo[0],q->socketinfo[1],q->socketinfo[2],q->socketinfo[3],q->socketinfo[4],q->anon_inodeinfo[0],q->anon_inodeinfo[1],q->anon_inodeinfo[2],q->anon_inodeinfo[3],q->anon_inodeinfo[4],q->pipeinfo,q->fileinfo,q->syscallnum[10],q->syscallnum[1],q->syscallnum[2],q->syscallnum[3],q->syscallnum[4],q->syscallnum[5],q->syscallnum[6],q->syscallnum[7],q->syscallnum[8],q->syscallnum[9],q->state);
            }
            else{
                sprintf(sql_insert,"insert into psinfo(domname,psid,psname,parentid,layer,gleaderid, prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,total_vm,stack_vm,inc_rchar,inc_wchar,inc_syscr,inc_syscw,inc_read_bytes,inc_write_bytes,inc_cancelled_write_bytes,start_time,realstart_time,totalfiles,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,totalsyscall,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation,state)values('%s','%d','%s','%d','%d','%d', '%d','%ld','%ld','%d','%d','%d','%d','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%lf','%lf','%d','%d', '%d','%d','%d','%d','%d','%d', '%d', '%d', '%d', '%d', '%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d');",vmivm->vmname,q->tspid,q->tsname,q->tsparent,q->tslayer,q->tsgroupleader,q->tsprio,q->tsinc_minflt,q->tsinc_majflt,q->tsinc_utime,q->tsinc_stime,q->tsmm_users,q->tsmm_count,q->tstotal_vm,q->tsstack_vm,q->tsinc_rchar,q->tsinc_wchar,q->tsinc_syscr,q->tsinc_syscw,q->tsinc_read_bytes,q->tsinc_write_bytes,q->tsinc_cancelled_write_bytes,q->tsstart_time/60000000000.0,q->tsrealstart_time/60000000000.0,q->tsfdnum,q->socketinfo[0],q->socketinfo[1],q->socketinfo[2],q->socketinfo[3],q->socketinfo[4],q->anon_inodeinfo[0],q->anon_inodeinfo[1],q->anon_inodeinfo[2],q->anon_inodeinfo[3],q->anon_inodeinfo[4],q->pipeinfo,q->fileinfo,q->syscallnum[10],q->syscallnum[1],q->syscallnum[2],q->syscallnum[3],q->syscallnum[4],q->syscallnum[5],q->syscallnum[6],q->syscallnum[7],q->syscallnum[8],q->syscallnum[9],q->state);

            }
            exec_db(mysql,sql_insert);
            insertsqlnum++;  
        }
        ///}
        ///printf("\n%s\n",sql_insert);
        ///freeq = q;
        q = q->next;
        
        ///free(freeq);
    }
    freeQueue(pre_queue);
    pre_queue->front->next = queue->front->next;
    pre_queue->rear = queue->rear;
    printf("insert sql num: %d\n",insertsqlnum);
}

int getParentLayer(int parentid,LinkQueue* queue)
{
    TaskNode* q = queue->front->next;
    while(q != NULL)
    {
        if(q->tspid == parentid){
            if(q->tslayer > -1){
                return q->tslayer;
            }
            else
            {
                if(q->tsparent == q->tspid){
                    q->tslayer = 0;
                    return 0;
                }
                else{
                    q->tslayer = getParentLayer(q->tsparent,queue)+1;
                    return q->tslayer;
                }
            }
        }
        q = q->next;
    }
    //** if can't find father make it layer 50*/
    return 49;
}

void setParentLayer(LinkQueue* queue)
{
    TaskNode* q = queue->front->next;
    while(q != NULL)
    {
        if(q->tsparent == q->tspid){
            q->tslayer = 0;
        }
        else{
            q->tslayer = getParentLayer(q->tsparent,queue)+1;
        }
        ///printf("%d parent:%d layer:%d\n",q->tspid,q->tsparent,q->tslayer);
        q = q->next;
    }
}
