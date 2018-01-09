/**
* Copyright(c) 2018 dyf
* implement vm read syscall config
*/

#include "vminit.h"
#include <stdio.h>

int read_version_conf(VmiInfo* vmiinfo)
{
    FILE *fp;
    char vmname[100] = "config/";
    strcat(vmname,vmiinfo->vmname);
    strcat(vmname,".conf");
    fp = fopen(vmname,"r");
    if(NULL == fp){
        printf("open %s fail\n",vmname);
        return 0;
    }
    if(EOF == fscanf(fp,"version = %s",vmiinfo->version))
    {
        printf("can't find version of %s\n",vmiinfo->vmname);
        return 0;
    }
    return 1;
}

int read_syscall_conf(VmiInfo* vmiinfo)
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
    ///printf("ready enter:%d\n",vmiinfo->syscall_len);
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

int read_offset_conf(VmiInfo* vmiinfo)
{
    vmiinfo->offset_len = 0;
    FILE *fp;
    char vmversion[100] = "config/";
    strcat(vmversion,vmiinfo->version);
    strcat(vmversion,"_offset.conf");
    ///printf("%s\n",vmversion);
    fp = fopen(vmversion,"r");
    if(NULL == fp){
        printf("open %s fail\n",vmversion);
        return 0;
    }
    char key[30];
    unsigned long value = 0;
    while(EOF != fscanf(fp,"%s = %ld%*[^\n]",key,&value)){
        strcpy(vmiinfo->vmoffset[vmiinfo->offset_len].key,key);
        vmiinfo->vmoffset[vmiinfo->offset_len].value = value;
        vmiinfo->offset_len++;
    }
    return 1;
}

VmiInfo* get_vmiinfo_vmi(vmi_instance_t vmi)
{
    unsigned long vmid = vmi_get_vmid(vmi);
    int i = 0;
    for(;i < MAX_VM; i++)
    {
        if(globalvm[i]->vmid == vmid){
            return globalvm[i];
        }
    }
    printf("can't find vm for id: %ld",vmid);
    return NULL;
}