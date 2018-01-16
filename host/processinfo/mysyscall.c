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

reg_t rdi = 0;
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
    /*
    if(testerror != 1)
    {
        printf("no trap !!!\n");
    }
    testerror = 0;
    */
   /// printf("enter one cb %d %x\n ",sys_num,trap);
    VmiInfo* vmivm = get_vmiinfo_vmi(vmi);
    //printf("modify vmiinfo:%s \n",vmivm->version);
    //vmi_write_8_va(vmi, vmivm->syscallall[sys_num].addr, 0, &trap);
    if(vmivm->syscallall[vmivm->syscall].sign == 1){
        vmi_write_8_va(vmi, vmivm->syscallall[vmivm->syscall].addr, 0, &trap);
    }
    ++singstepnum;
 

   /// printf("%ld one step \n",rax);

    return VMI_EVENT_RESPONSE_TOGGLE_SINGLESTEP;
}

event_response_t trap_cb(vmi_instance_t vmi, vmi_event_t *event)
{
    ///printf("enter trap cb---");
    

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

    if(rax == 2){
        FILE *pf = fopen("log/file.log","a");
        uint64_t rbx = 0;
        uint64_t rcx = 0;
        uint64_t rdx = 0;
        uint64_t rdi = 0;
        uint64_t rsi = 0;
        uint64_t r8 = 0;
        uint64_t r9 = 0;
        uint64_t r10 = 0;
        vmi_get_vcpureg(vmi, &rbx, RBX, event->vcpu_id);
        vmi_get_vcpureg(vmi, &rcx, RCX, event->vcpu_id);
        vmi_get_vcpureg(vmi, &rdx, RDX, event->vcpu_id);
        vmi_get_vcpureg(vmi, &rdi, RDI, event->vcpu_id);
        vmi_get_vcpureg(vmi, &rsi, RSI, event->vcpu_id);
        vmi_get_vcpureg(vmi, &r8, R8, event->vcpu_id);
        vmi_get_vcpureg(vmi, &r9, R9, event->vcpu_id);
        vmi_get_vcpureg(vmi, &r10, R10, event->vcpu_id);
        
        char file_name[255] = "";
        char c = 33;
        addr_t filenameadd = rbx;
        int n = 50;
        while (c != '\0'&&(c<127&&c>32)&&n>0)
        {
            n--;
            vmi_read_8_va(vmi, filenameadd, pid, &c);
            filenameadd = filenameadd + 1;
            printf("%c",c);
            char c_rmp[2] = {c};
            strcat(file_name, c_rmp);
        }
        printf("\n");
        fprintf(pf,"%d open rbx:%lx rcv:%lx rdx:%lx rsi:%lx rdi:%lx r8:%lx r9:%lx r10:%lx\n",pid,rbx,rcx,rdx,rsi,rdi,r8,r9,r10);
        fprintf(pf,"%d open rbx:%ld rcv:%ld rdx:%ld rsi:%ld rdi:%ld r8:%ld r9:%ld r10:%ld\n\n",pid,rbx,rcx,rdx,rsi,rdi,r8,r9,r10);
        
        fclose(pf);
    }
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

