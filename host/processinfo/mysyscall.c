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
    /*
    if(testerror != 1)
    {
        printf("no trap !!!\n");
    }
    testerror = 0;
    */
    VmiInfo* vmivm = get_vmiinfo_vmi(vmi);
   /// printf("enter one cb %d \n ",vmivm->syscall);
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
    uint64_t r10 = 0;
    uint64_t rdx = 0;
    uint64_t rdi = 0;
    uint64_t rsi = 0;
    switch(rax)
    {
    case 2:
        vmi_get_vcpureg(vmivm->vmi, &rdx, RDX, 0);
        vmi_get_vcpureg(vmivm->vmi, &rdi, RDI, 0);
        vmi_get_vcpureg(vmivm->vmi, &rsi, RSI, 0);
        if(rdi < 0x7ff000000000){
            char file_name2[255] = "";
            addr_t filenameadd2 = rdi;
            
            if(1 == get_file_string(vmivm->vmi,filenameadd2,pid,file_name2))
            {
                /*
                unsigned int rdxvalue = 0;
                addr_t filenameadd5 = rdx;
                vmi_read_32_va(vmivm->vmi, filenameadd5, pid, &rdxvalue);
                */
                //fprintf(pf,"%d open rbx:%lx rcx:%lx rdx:%lx rsi:%lx rdi:%lx\n",pid,rbx,rcx,rdx,rsi,rdi);
                //printf("%d open %s mode:%lx flags:%x\n", pid, file_name2, rsi, rdxvalue);
                ///printf("%lx\n",rdi);
                fprintf(pf, "%d open %s mode:%lx fileaddr:%lx\n", pid, file_name2, rsi,rdi);
            }
        }
        break;
    case 43:
        fprintf(pf, "%d connect \n", pid);
        break;
    case 59:
        vmi_get_vcpureg(vmivm->vmi, &rdx, RDX, 0);
        vmi_get_vcpureg(vmivm->vmi, &rdi, RDI, 0);
        vmi_get_vcpureg(vmivm->vmi, &rsi, RSI, 0);
        
        addr_t  filenameadd2= rdi;
        ///printf("%lx\n",rdi);
        char rdifile[255] = "";
        if(1 == get_file_string(vmivm->vmi,filenameadd2,pid,rdifile))
        {
            fprintf(pf,"%d exe %s\n",pid,rdifile);
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
        fprintf(pf,"%d kill %ld\n",pid,rdi);
        printf("%d kill %ld\n",pid,rdi);
        break;
    case 87:
        vmi_get_vcpureg(vmivm->vmi, &rdi, RDI, 0);
        addr_t  unlink_pathaddr= rdi;
        ///printf("%lx\n",rdi);
        char unlink_path[255] = "";
        if(1 == get_file_string(vmivm->vmi,unlink_pathaddr,pid,unlink_path))
        {
            fprintf(pf,"%d unlink %s\n",pid,unlink_path);
            printf("%d unlink %s\n",pid,unlink_path);
        }
        break;
    case 176:
        fprintf(pf,"%d module delete\n",pid);
        printf("%d module delete\n",pid);
        break;
    case 231:
        fprintf(pf,"%d group exit\n",pid);
        printf("%d group exit\n",pid);
        break;
    case 313:
        fprintf(pf,"%d module init\n",pid);
        printf("%d module init\n",pid);
        break;
    default:
        //fprintf(pf,"%d %ld\n",pid,rax);
        //printf("%d %ld\n",pid,rax);
        break;
    }
    fclose(pf);
}
