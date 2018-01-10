/**
* Copyright(c) 2018 dyf
* .h vm read syscall config
*/

#ifndef VMINIT_H_INCLUDED
#define VMINIT_H_INCLUDED

#include <libvmi/libvmi.h>
#include <stdio.h>

#define NUMBER_OF_OFFSET 57

typedef struct offsetkv
{
    char key[30];
    unsigned long value;
}OffSet;

typedef struct syscallone
{
    int num;
    uint64_t addr;
    char name[30];
    uint8_t pre;
    ///(1)ps_control (2)file_rw (3)file_control (4)sys_control (5)mem_control (6)net_control (7)socket_control (8)user_control (9)ps_communcation
    int classify;
    int sign;
}SyscallOne;

typedef struct vmiinfo
{
    vmi_instance_t vmi;
    char vmname[50];
    unsigned long vmid;
    char version[50];
    int syscall_len;
    SyscallOne* syscallall;
    //OffSet vmoffset[NUMBER_OF_OFFSET];
    int offset_len;
    unsigned long vmoffset[NUMBER_OF_OFFSET];
    reg_t syscall;
}VmiInfo;

enum
{
    pid_offset, name_offset, socket_sk_offset, sk_daddr_offset, sk_rcv_saddr_offset, sk_num_offset, sk_dport_offset, sk_v6_daddr_offset, sk_v6_rcv_saddr_offset, fs_offset, tgid_offset, fsroot_offset, fspwd_offset, files_offset, fdt_offset, maxfds_offset, nameidata_offset, task_real_parent_offset, task_parent_offset, group_leader_offset, start_time_offset, real_start_time_offset, min_flt_offset, maj_flt_offset, utime_offset, stime_offset, gtime_offset, ioac_offset, prio_offset, static_prio_offset, normal_prio_offset, rt_priority_offset, mm_offset, pgd_offset, mm_users_offset, mm_count_offset, total_vm_offset, locked_vm_offset, stack_vm_offset, start_code_offset, end_code_offset, fd_offset, i_node_offset, i_uid_offset, i_gid_offset, private_date_offset, f_mode_offset, f_flags_offset, path_offset, dentry_offset, parent_offset, dname_offset, qname_offset, di_name_offset, nameidata_last_offset, nameidata_root_offset, nameidata_path_offset
}VmOffset;

#define MAX_VM 10
VmiInfo* globalvm[MAX_VM];
int vm_num;

int read_version_conf(VmiInfo* vmiinfo);

int read_syscall_conf(VmiInfo* vmiinfo);

int read_offset_conf(VmiInfo* vmiinfo);

VmiInfo* get_vmiinfo_vmi(vmi_instance_t vmi);

int set_offset(VmiInfo* vmiinfo,char* offsetname,unsigned long value);

#endif