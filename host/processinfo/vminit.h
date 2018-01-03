/**
* Copyright(c) 2018 dyf
* implement vm read syscall config
*/
#ifndef VMINIT_H_INCLUDED
#define VMINIT_H_INCLUDED
#include <libvmi/libvmi.h>
#include <stdio.h>
#include "mysyscall.h"

typedef struct vmiinfo{
    vmi_instance_t vmi;
    char version[50];
    int syscall_len;
    SyscallOne* syscallall;
    int offset_len;
}VmiInfo;


int readsyscallconf(VmiInfo* vmiinfo)
{
    FILE *fp;
    char vmversion[100] = "config/";
    strcat(vmversion,vmiinfo->version);
    strcat(vmversion,"_syscall.conf");
    fp = fopen(vmversion,"r");
    if(NULL == fp){
        printf("open %s fail\n",vmversion);
        return 0;
    }

    int numofread = fscanf(fp,"%d%*[^\n]",&(vmiinfo->syscall_len));
    SyscallOne* syscallinfo = (SyscallOne*)malloc((vmiinfo->syscall_len+1)*sizeof(SyscallOne));

    int i = 0;
    printf("ready enter:%d\n",vmiinfo->syscall_len);
    while(i-1 < vmiinfo->syscall_len){
        numofread = fscanf(fp,"%d %lx %s %d%*[^\n]",&(syscallinfo[i].num),&(syscallinfo[i].addr),syscallinfo[i].name,&syscallinfo[i].classify);
        syscallinfo[i].pre = 0x00;
        SyscallOne* temp = vmiinfo->syscallall;
        i++;
    }
    vmiinfo->syscallall = syscallinfo;
    /*
    i = 0;
    for(;i<vmiinfo->syscall_len;i++){
        printf("%d %lx %s %d\n",vmiinfo->syscallall[i].num,vmiinfo->syscallall[i].addr,vmiinfo->syscallall[i].name,vmiinfo->syscallall[i].classify);
    }
    */
    return 1;
}
#endif