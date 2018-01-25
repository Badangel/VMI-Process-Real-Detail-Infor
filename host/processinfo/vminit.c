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
        syscallinfo[i].sign = 0;
        numofread = fscanf(fp,"%d %lx %s %d %d%*[^\n]",&(syscallinfo[i].num),&(syscallinfo[i].addr),syscallinfo[i].name,&syscallinfo[i].classify,&syscallinfo[i].sign);
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
        //strcpy(vmiinfo->vmoffset[vmiinfo->offset_len].key,key);
       // vmiinfo->vmoffset[vmiinfo->offset_len].value = value;
        set_offset(vmiinfo,key,value);
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
int set_offset(VmiInfo* vmiinfo,char* offsetname,unsigned long value)
{
    if(strcmp(offsetname, "pid_offset") == 0)
    {
        vmiinfo->vmoffset[pid_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "name_offset") == 0)
    {
        vmiinfo->vmoffset[name_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "socket_sk_offset") == 0)
    {
        vmiinfo->vmoffset[socket_sk_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "sk_daddr_offset") == 0)
    {
        vmiinfo->vmoffset[sk_daddr_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "sk_rcv_saddr_offset") == 0)
    {
        vmiinfo->vmoffset[sk_rcv_saddr_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "sk_num_offset") == 0)
    {
        vmiinfo->vmoffset[sk_num_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "sk_dport_offset") == 0)
    {
        vmiinfo->vmoffset[sk_dport_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "sk_v6_daddr_offset") == 0)
    {
        vmiinfo->vmoffset[sk_v6_daddr_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "sk_v6_rcv_saddr_offset") == 0)
    {
        vmiinfo->vmoffset[sk_v6_rcv_saddr_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "fs_offset") == 0)
    {
        vmiinfo->vmoffset[fs_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "tgid_offset") == 0)
    {
        vmiinfo->vmoffset[tgid_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "fsroot_offset") == 0)
    {
        vmiinfo->vmoffset[fsroot_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "fspwd_offset") == 0)
    {
        vmiinfo->vmoffset[fspwd_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "files_offset") == 0)
    {
        vmiinfo->vmoffset[files_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "fdt_offset") == 0)
    {
        vmiinfo->vmoffset[fdt_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "maxfds_offset") == 0)
    {
        vmiinfo->vmoffset[maxfds_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "nameidata_offset") == 0)
    {
        vmiinfo->vmoffset[nameidata_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "task_real_parent_offset") == 0)
    {
        vmiinfo->vmoffset[task_real_parent_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "task_parent_offset") == 0)
    {
        vmiinfo->vmoffset[task_parent_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "group_leader_offset") == 0)
    {
        vmiinfo->vmoffset[group_leader_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "start_time_offset") == 0)
    {
        vmiinfo->vmoffset[start_time_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "real_start_time_offset") == 0)
    {
        vmiinfo->vmoffset[real_start_time_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "min_flt_offset") == 0)
    {
        vmiinfo->vmoffset[min_flt_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "maj_flt_offset") == 0)
    {
        vmiinfo->vmoffset[maj_flt_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "utime_offset") == 0)
    {
        vmiinfo->vmoffset[utime_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "stime_offset") == 0)
    {
        vmiinfo->vmoffset[stime_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "gtime_offset") == 0)
    {
        vmiinfo->vmoffset[gtime_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "ioac_offset") == 0)
    {
        vmiinfo->vmoffset[ioac_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "prio_offset") == 0)
    {
        vmiinfo->vmoffset[prio_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "static_prio_offset") == 0)
    {
        vmiinfo->vmoffset[static_prio_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "normal_prio_offset") == 0)
    {
        vmiinfo->vmoffset[normal_prio_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "rt_priority_offset") == 0)
    {
        vmiinfo->vmoffset[rt_priority_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "mm_offset") == 0)
    {
        vmiinfo->vmoffset[mm_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "pgd_offset") == 0)
    {
        vmiinfo->vmoffset[pgd_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "mm_users_offset") == 0)
    {
        vmiinfo->vmoffset[mm_users_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "mm_count_offset") == 0)
    {
        vmiinfo->vmoffset[mm_count_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "total_vm_offset") == 0)
    {
        vmiinfo->vmoffset[total_vm_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "locked_vm_offset") == 0)
    {
        vmiinfo->vmoffset[locked_vm_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "stack_vm_offset") == 0)
    {
        vmiinfo->vmoffset[stack_vm_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "start_code_offset") == 0)
    {
        vmiinfo->vmoffset[start_code_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "end_code_offset") == 0)
    {
        vmiinfo->vmoffset[end_code_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "fd_offset") == 0)
    {
        vmiinfo->vmoffset[fd_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "i_node_offset") == 0)
    {
        vmiinfo->vmoffset[i_node_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "i_uid_offset") == 0)
    {
        vmiinfo->vmoffset[i_uid_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "i_gid_offset") == 0)
    {
        vmiinfo->vmoffset[i_gid_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "private_date_offset") == 0)
    {
        vmiinfo->vmoffset[private_date_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "f_mode_offset") == 0)
    {
        vmiinfo->vmoffset[f_mode_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "f_flags_offset") == 0)
    {
        vmiinfo->vmoffset[f_flags_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "path_offset") == 0)
    {
        vmiinfo->vmoffset[path_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "dentry_offset") == 0)
    {
        vmiinfo->vmoffset[dentry_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "parent_offset") == 0)
    {
        vmiinfo->vmoffset[parent_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "dname_offset") == 0)
    {
        vmiinfo->vmoffset[dname_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "qname_offset") == 0)
    {
        vmiinfo->vmoffset[qname_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "di_name_offset") == 0)
    {
        vmiinfo->vmoffset[di_name_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "nameidata_last_offset") == 0)
    {
        vmiinfo->vmoffset[nameidata_last_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "nameidata_root_offset") == 0)
    {
        vmiinfo->vmoffset[nameidata_root_offset] = value;
        return 1;
    }
    if(strcmp(offsetname, "nameidata_path_offset") == 0)
    {
        vmiinfo->vmoffset[nameidata_path_offset] = value;
        return 1;
    }
    return 0;
}

int get_file_string(vmi_instance_t vmi, addr_t startaddr, pid_t pid,char *mystring)
{
    char c = 0;
    int boolc = 0;
    do
    {
        if(VMI_SUCCESS == vmi_read_8_va(vmi, startaddr, pid, &c))
        {
            startaddr = startaddr + 1;
            char c_rmp[2] = {c};
            strcat(mystring, c_rmp);
            boolc = 1;
        }
    }while (c != '\0' && (c < 127 && c > 32));
    return boolc;
}