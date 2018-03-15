/**
* Copyright(c) 2017 dyf
* record and modify syscall enter addr.
* defind pipe transmit syscall format
* interrupt to recover enter addr
*/

#include <signal.h>
#include "mysyscall.h"

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

    vmi_get_vcpureg(vmi, &rax, RAX, event->vcpu_id);
    VmiInfo* vmivm = get_vmiinfo_vmi(vmi);
    int singcbsysnum = (unsigned int)rax;

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
    ///printf("vmi addr:%lx\n",&vmi);

    vmi_get_vcpureg(vmi, &rax, RAX, event->vcpu_id);
    vmi_get_vcpureg(vmi, &cr3, CR3, event->vcpu_id);
    vmi_pid_t pid = vmi_dtb_to_pid(vmi, cr3);

    psyscall nowsyscall;
    nowsyscall.pid = pid;
    nowsyscall.sysnum = (unsigned int)rax;

    record_syscall(vmivm,rax,pid);

    writen = write(pipenum, &nowsyscall, sizeof(psyscall));
    if(writen<1){
        printf("pipe write error!\n");
    }

    //sys_num = rax;
    vmivm->syscall = rax;
    syscallnum[vmivm->syscall]++;

    ///printf("%s %d syscall#=%u right:%x\n",vmivm->version, pid,(unsigned int)rax,vmivm->syscallall[vmivm->syscall].pre);
    vmi_write_8_va(vmi, vmivm->syscallall[vmivm->syscall].addr, 0, &(vmivm->syscallall[vmivm->syscall].pre));

    event->interrupt_event.reinject = 0;
    ///printf("return\n");

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

void record_syscall(VmiInfo* vmivm, reg_t rax,vmi_pid_t pid)
{
    FILE *pf = fopen("log/file.log","a");
    uint64_t r8 = 0;
    uint64_t r9 = 0;
    uint64_t r10 = 0;
    uint64_t rdx = 0;
    uint64_t rdi = 0;
    uint64_t rsi = 0;
    char psname[80]="";
    get_psname_by_pid(vmivm,pid,psname);
    switch(rax)
    {
    case 2:
        vmi_get_vcpureg(vmivm->vmi, &rdi, RDI, 0);
        vmi_get_vcpureg(vmivm->vmi, &rsi, RSI, 0);
        addr_t rdifilenameaddr = rdi; 
        char* rdifilename = vmi_read_str_va(vmivm->vmi,rdifilenameaddr,pid);
        if(rdifilename!=NULL){
            fprintf(pf, "%s(%d) %s(%ld)open rdi:%s rsi:%lx\n", psname,pid,vmivm->syscallall[rax].name,rax, rdifilename, rsi);
            free(rdifilename);
        }
        break;
    case 3:
        vmi_get_vcpureg(vmivm->vmi, &rdi, RDI, 0);
        fprintf(pf, "%s(%d) %s(%ld)close rdi:%ld \n", psname,pid,vmivm->syscallall[rax].name,rax, rdi);
        break;
    case 43:
        fprintf(pf, "%s(%d) %s(%ld) connect\n", psname,pid,vmivm->syscallall[rax].name,rax);
        break;
    case 59:
        vmi_get_vcpureg(vmivm->vmi, &rdx, RDX, 0);
        vmi_get_vcpureg(vmivm->vmi, &rdi, RDI, 0);
        vmi_get_vcpureg(vmivm->vmi, &rsi, RSI, 0);
        
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
        vmi_get_vcpureg(vmivm->vmi, &rdi, RDI, 0);
        fprintf(pf,"%s(%d) %s(%ld)kill %ld\n",psname,pid,vmivm->syscallall[rax].name,rax,rdi);
        printf("%s(%d) %s(%ld)kill %ld\n",psname,pid,vmivm->syscallall[rax].name,rax,rdi);
        break;
    case 87:
        vmi_get_vcpureg(vmivm->vmi, &rdi, RDI, 0);
        addr_t  unlink_pathaddr= rdi;
        ///printf("%lx\n",rdi);
        char* unlink_path = vmi_read_str_va(vmivm->vmi,unlink_pathaddr,pid);
        if(unlink_path!=NULL){
            fprintf(pf,"%s(%d) %s(%ld)unlink %s\n",psname,pid,vmivm->syscallall[rax].name,rax,unlink_path);
            printf("%s(%d) %s(%ld)unlink %s\n",psname,pid,vmivm->syscallall[rax].name,rax,unlink_path);
            free(unlink_path);
        }
        break;
    case 176:
        fprintf(pf,"%s(%d) %s(%ld) module delete\n",psname,pid,vmivm->syscallall[rax].name,rax);
        printf("%s(%d) %s(%ld)  module delete\n",psname,pid,vmivm->syscallall[rax].name,rax);
        break;
    case 231:
        fprintf(pf,"%s(%d) %s(%ld) group exit\n",psname,pid,vmivm->syscallall[rax].name,rax);
        printf("%s(%d) %s(%ld) group exit\n",psname,pid,vmivm->syscallall[rax].name,rax);
        break;
    case 313:
        fprintf(pf,"%s(%d) %s(%ld) module init\n",psname,pid,vmivm->syscallall[rax].name,rax);
        printf("%s(%d) %s(%ld) module init\n",psname,pid,vmivm->syscallall[rax].name,rax);
        break;
    default:
        fprintf(pf,"%s(%d) %s(%ld) \n",psname,pid,vmivm->syscallall[rax].name,rax);
        //printf("%d %ld\n",pid,rax);
        break;
    }
    fclose(pf);
}
