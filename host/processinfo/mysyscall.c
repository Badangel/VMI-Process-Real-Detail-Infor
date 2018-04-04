/**
* Copyright(c) 2017 dyf
* record and modify syscall enter addr.
* defind pipe transmit syscall format
* interrupt to recover enter addr
*/

#include <signal.h>
#include "mysyscall.h"
#include "mypsinfo.h"
#include "mymodinfo.h"

/* Signal handler */
void close_handler(int sig)
{
    interrupted = sig;
}
int syscallnum[NUMBER_OF_SYSCALLS] = {0};

reg_t rax = 0;
reg_t cr3 = 0;

uint8_t trap = 0xCC;
int trapnum = 0;
int singstepnum = 0;
int sys_num = -1;
//int pipenum = -1;
int writen = -1;
int testerror = 0;

event_response_t singlestep_cb(vmi_instance_t vmi, vmi_event_t *event)
{
    //rax = event->x86_regs->rax;
    VmiInfo* vmivm = get_vmiinfo_vmi(vmi);
    int singcbsysnum = vmivm->syscall;
    //printf("single rip:%lx\n",event->x86_regs->rip);

   /// printf("enter one cb %d \n ",vmivm->syscall);
    //printf("modify vmiinfo:%s \n",vmivm->version);
    //vmi_write_8_va(vmi, vmivm->syscallall[sys_num].addr, 0, &trap);
    if(vmivm->syscallall[singcbsysnum].sign == 1){
       vmi_write_8_va(vmi, vmivm->syscallall[singcbsysnum].addr, 0, &trap);
    }
    ++singstepnum;
   /// printf("%ld one step \n",rax);
    return VMI_EVENT_RESPONSE_TOGGLE_SINGLESTEP;
}

event_response_t trap_cb(vmi_instance_t vmi, vmi_event_t *event)
{
    ///printf("enter trap cb-----------------\n");
    

    VmiInfo* vmivm = get_vmiinfo_vmi(vmi);
    /*
    if(testerror != 0)
    {
        printf("no singlestep %ld!!!\n",vmivm->syscall);
    }
    testerror = 1;
    */
    rax = event->x86_regs->rax;
    cr3 = event->x86_regs->cr3;
    PsNode* nowps = get_ps_fron_pgd(vmivm,cr3);
    char *psname = NULL;
    vmi_pid_t pid =-1;
    addr_t currentpsaddr = 0;
   // FILE *pf = fopen("log/file.log","a");
    if (nowps == NULL)
    {
        //printf("PsNode is NULL\n");
        pid = vmi_dtb_to_pid(vmi, cr3);
        nowps = get_ps_fron_pid(vmivm,pid);
        char psnamea[80] = "";
        currentpsaddr = get_psname_by_pid(vmivm, pid, psnamea);
        psname = psnamea;
        PsNode *oneps = malloc(sizeof(PsNode));
        strcpy(oneps->name, psname);
        oneps->pid = pid;
        oneps->addr = currentpsaddr;
        oneps->pgd = cr3;
        //printf("22\n");
        add_pslist(vmivm, oneps);
        //printf("33\n");
        vmivm->ps_num++;
        printf("Add %s(%d) pgd:%lx\n", psname, pid, cr3);

        record_ps_list(vmivm);
        //printf("55\n");     
    }
    else
    {
        pid = nowps->pid;
        get_psname_by_addr(vmivm,nowps->name,nowps->addr);
        psname = nowps->name;
        currentpsaddr = nowps->addr;
       // fprintf(pf,"find %s(%d) pgd:%lx\n", psname, pid, cr3);
    }
    
    //fclose(pf);
    //printf("33\n");
    uint64_t value_rip = 0;
    uint64_t rip = event->x86_regs->rip;
    vmi_read_64_va(vmi,rip,0,&value_rip);
    int rip_rax = rax;

    if(rip_rax<0||vmivm->syscallall[rip_rax].addr != rip){
        rip_rax = get_sysnum_from_rip(vmivm,rip);
        //printf("rax:%ld rip-rax:%d rip:%lx value:%lx\n", event->x86_regs->rax, rip_rax, rip, value_rip);
    }
    psyscall nowsyscall;
    nowsyscall.pid = pid;
    nowsyscall.sysnum = rip_rax;
    event->interrupt_event.reinject = 0;

    if (vmivm->syscallall[nowsyscall.sysnum].sign == 0)
    {
        uint8_t nowadd = 0;
        vmi_read_8_va(vmi, vmivm->syscallall[nowsyscall.sysnum].addr, 0, &nowadd);
        printf("error int3!!!!%d!!!!!!8bit:%x\n", nowsyscall.sysnum, nowadd);
        detect_syscall_hook(vmivm);
        return VMI_EVENT_RESPONSE_EMULATE;
    }
    if(trapnum%10000==0){
        detect_syscall_hook(vmivm);
    }
    record_syscall(vmivm,rip_rax,pid,psname,currentpsaddr,event);
   // printf("44\n");
    writen = write(pipenum, &nowsyscall, sizeof(psyscall));
    if (writen < 1)
    {
        printf("pipe write error!\n");
    }

    
    //sys_num = rax;
    vmivm->syscall = rip_rax;
    syscallnum[vmivm->syscall]++;
    ///printf("%s %d syscall#=%u right:%x\n",vmivm->version, pid,(unsigned int)rax,vmivm->syscallall[vmivm->syscall].pre);
    vmi_write_8_va(vmi, vmivm->syscallall[vmivm->syscall].addr, 0, &(vmivm->syscallall[vmivm->syscall].pre));
    ++trapnum;

    return VMI_EVENT_RESPONSE_TOGGLE_SINGLESTEP;
}

void combineSyscallAndPs(LinkQueue* queue,int sysnum[][11],int psnum){
    int i = 0;
    TaskNode* q = queue->front->next;
    while(q != NULL)
    {
        int j = 1;
        int totalsyscall = 0;
        for(; j<11; j++)
        {
            q->syscallnum[j-1] = sysnum[i][j];
            totalsyscall+=sysnum[i][j];
        }
        q->syscallnum[10] = totalsyscall;
        i++;
        q = q->next;
    }
}

void find_syscall_hook(VmiInfo* vmivm,MYSQL* mysql,int sysnum,uint64_t backup_byte){
    FILE *pf = fopen("log/warning.log","a");
    fprintf(pf, "%s(%d:%lx) is hooked to %lx!! \n", vmivm->syscallall[sysnum].name,sysnum,vmivm->syscallall[sysnum].addr,backup_byte);
    char sql_insert[1024];
    sprintf(sql_insert,"insert into warning(domname,class,pmname)values('%s','%s','%s');",vmivm->vmname,"Syscall Hook",vmivm->syscallall[sysnum].name);
    exec_db(mysql,sql_insert);
    fclose(pf);
}

void record_syscall(VmiInfo* vmivm, reg_t rax,vmi_pid_t pid,char* psname,addr_t currentpsaddr,vmi_event_t *event)
{
    FILE *pf = fopen("log/file.log","a");
    uint64_t r8 = 0;
    uint64_t r9 = 0;
    uint64_t r10 = 0;
    uint64_t rdx = 0;
    uint64_t rdi = 0;
    uint64_t rsi = 0;
    //char psname[80]="";
    //get_psname_by_pid(vmivm,pid,psname);
    switch(rax)
    {
    case 2:
        rdi = event->x86_regs->rdi;
        rsi = event->x86_regs->rsi;
        addr_t rdifilenameaddr2 = rdi; 
        char* rdifilename2 = vmi_read_str_va(vmivm->vmi,rdifilenameaddr2,pid);
        if(rdifilename2!=NULL){
            fprintf(pf, "%s(%d) %s(%ld)open rdi:%s rsi:%lx\n", psname,pid,vmivm->syscallall[rax].name,rax, rdifilename2, rsi);
            free(rdifilename2);
        }
        break;
    case 3:
        rdi = event->x86_regs->rdi;
        fprintf(pf, "%s(%d) %s(%ld)close rdi:%ld \n", psname,pid,vmivm->syscallall[rax].name,rax, rdi);
        break;
    case 42:
        rdi = event->x86_regs->rdi;
        //SocketSR* sock = malloc(sizeof(SocketSR));
        //strcpy(sock->sendaddr,"1.1.1.1");
        //strcpy(sock->recvaddr,"1.1.1.1");
        //get_socket_info(vmivm,currentpsaddr,rdi,sock);
        //fprintf(pf, "%s(%d) %s(%ld) connect fd:%ld %s->%s\n", psname,pid,vmivm->syscallall[rax].name,rax,rdi,sock->sendaddr,sock->recvaddr);
        fprintf(pf, "%s(%d) %s(%ld) connect fd:%ld\n", psname,pid,vmivm->syscallall[rax].name,rax,rdi);
       // free(sock);
        break;
    case 59:
        rdi = event->x86_regs->rdi;
        addr_t  filenameadd2= rdi;
        ///printf("%lx\n",rdi);
        char* rdifile = vmi_read_str_va(vmivm->vmi,filenameadd2,pid);
        if(rdifile!=NULL){
            fprintf(pf,"%s(%d) %s(%ld)exe %s\n",psname,pid,vmivm->syscallall[rax].name,rax,rdifile);
            free(rdifile);
        }
        break;
    /*call exit for children
    case 60:
        fprintf(pf,"%d exit\n",pid);
        printf("%d exit\n",pid);
        break;
        */
    case 62:
        rdi = event->x86_regs->rdi;
        fprintf(pf,"%s(%d) %s(%ld)kill %ld\n",psname,pid,vmivm->syscallall[rax].name,rax,rdi);
        //printf("%s(%d) %s(%ld)kill %ld\n",psname,pid,vmivm->syscallall[rax].name,rax,rdi);
        if(pid<=rdi)
        {
            delete_one_ps(vmivm,rdi);
            FILE *ep = fopen(vmivm->exitpsfile,"a");
            char psnamea[80] = "";
            get_psname_by_pid(vmivm, rdi, psnamea);
            fprintf(ep,"%ld %s\n",rdi,psnamea);
            fclose(ep);
        }
        break;
    case 87:
        rdi = event->x86_regs->rdi;
        addr_t  unlink_pathaddr= rdi;
        ///printf("%lx\n",rdi);
        char* unlink_path = vmi_read_str_va(vmivm->vmi,unlink_pathaddr,pid);
        if(unlink_path!=NULL){
            get_module_name(vmivm,unlink_path);
            fprintf(pf,"%s(%d) %s(%ld)unlink %s\n",psname,pid,vmivm->syscallall[rax].name,rax,unlink_path);
            //printf("%s(%d) %s(%ld)unlink %s\n",psname,pid,vmivm->syscallall[rax].name,rax,unlink_path);
            free(unlink_path);
        }
        break;
    case 176:
        fprintf(pf,"%s(%d) %s(%ld) module delete\n",psname,pid,vmivm->syscallall[rax].name,rax);
        //printf("%s(%d) %s(%ld)  module delete\n",psname,pid,vmivm->syscallall[rax].name,rax);
        break;
    case 231:
        fprintf(pf,"%s(%d) %s(%ld) group exit\n",psname,pid,vmivm->syscallall[rax].name,rax);
        //printf("%s(%d) %s(%ld) group exit\n",psname,pid,vmivm->syscallall[rax].name,rax);
        FILE *ep = fopen(vmivm->exitpsfile,"a");
        delete_one_ps(vmivm,pid);
        fprintf(ep,"%d %s\n",pid,psname);
        fclose(ep);
        break;
    case 313:
        fprintf(pf,"%s(%d) %s(%ld) module init\n",psname,pid,vmivm->syscallall[rax].name,rax);
        //printf("%s(%d) %s(%ld) module init\n",psname,pid,vmivm->syscallall[rax].name,rax);
        //MyList * initmod_list= createMySearchList(compare2mod);
        //int initmod_num = get_module_info(vmivm,initmod_list);
        //freeMyList(initmod_list);
       /* vmi_get_vcpureg(vmivm->vmi, &rdi, RDI, 0);
        vmi_get_vcpureg(vmivm->vmi, &rsi, RSI, 0);
       
        vmi_get_vcpureg(vmivm->vmi, &rdx, RDX, 0);
        
        vmi_get_vcpureg(vmivm->vmi, &r8, R8, 0);
        vmi_get_vcpureg(vmivm->vmi, &r9, R9, 0);
        vmi_get_vcpureg(vmivm->vmi, &r10, R10, 0);
        //if(rdi < 0x7ff000000000){
        addr_t rdifilenameaddr = rdi;
        addr_t rsifilenameaddr = rsi;
        addr_t rdxfilenameaddr = rdx;
        addr_t r10filenameaddr = r10;
        addr_t r8filenameaddr = r8;
        addr_t r9filenameaddr = r9;
 
        char* rdifilename = vmi_read_str_va(vmivm->vmi,rdifilenameaddr,0);
        char* rsifilename = vmi_read_str_va(vmivm->vmi,rsifilenameaddr,0);
        char* rdxfilename = vmi_read_str_va(vmivm->vmi,rdxfilenameaddr,0);
        char* r10filename = vmi_read_str_va(vmivm->vmi,r10filenameaddr,0);
        char* r8filename = vmi_read_str_va(vmivm->vmi,r8filenameaddr,0);
        char* r9filename = vmi_read_str_va(vmivm->vmi,r9filenameaddr,0);
        fprintf(pf, "%d open rdi:%ld rsi:%ld rdx:%ld r8:%s r9:%s r10:%s\n", pid, rdi, rsi,rdx,r10filename,r8filename,r9filename);
		free(rdifilename);
        free(rsifilename);
        free(rdxfilename);
        free(r10filename);
        free(r8filename);
        free(r9filename);*/
        break;
    default:
        //fprintf(pf,"%s(%d) %s(%ld) \n",psname,pid,vmivm->syscallall[rax].name,rax);
        //printf("%d %ld\n",pid,rax);
        break;
    }
    fclose(pf);
}

void get_module_name(VmiInfo* vmivm,char* unlink_path){
    int i =0;
    int len =strlen(unlink_path);
    for(;i<len;i++){
        if(unlink_path[i]=='/'&&unlink_path[i+1]=='+'){
            char modname[50]="";
            strcpy(modname,unlink_path+i+9);
            printf("module name is %s!!\n",modname);
            
            FILE *pf = fopen(vmivm->modl,"w");
            fprintf(pf,"%s\n", modname);
            fclose(pf);
        }
    }

}

void detect_syscall_hook(VmiInfo* vmivm){
    printf("start detcet syscall hook!\n");
    FILE *pftest = fopen("log/test.log", "a");
    int i;
    // for(i = 0; i < vmivm->syscall_len; i++)
    int endsysnum = vmivm->syscall_len;
    addr_t sys_call_table_addr = 0xffffffff81a001c0;
    uint64_t sysaddr = 0;
    for (i = 0; i < endsysnum; i++)
    {
        if (vmivm->syscallall[i].addr != 0)
        {
            vmi_read_64_va(vmivm->vmi, sys_call_table_addr + i * 8, 0, &sysaddr);
            
            if (vmivm->syscallall[i].addr != sysaddr)
            {
                fprintf(pftest, "%d %s read in right addr:%lx was hooked to %lx!!\n", i, vmivm->syscallall[i].name, vmivm->syscallall[i].reallyaddr, sysaddr);
                printf("%d %s right addr:%lx was hooked to %lx!!\n", i, vmivm->syscallall[i].name, vmivm->syscallall[i].addr, sysaddr);
                vmi_write_64_va(vmivm->vmi, sys_call_table_addr + i * 8, 0, &(vmivm->syscallall[i].addr));
                printf("recover %s into right addr!\n", vmivm->syscallall[i].name);
                //find_syscall_hook(vmivm, &mysql, i, sysaddr);
            }
        }
      
    }
    fclose(pftest);
}

int get_sysnum_from_rip(VmiInfo* vmivm,uint64_t rip){
    int i =0;
    for (; i < vmivm->syscall_len; i++)
    {
        if(vmivm->syscallall[i].addr==rip){
            return i;
        }
    }
    return -1;
}