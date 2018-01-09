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
}SyscallOne;

typedef struct vmiinfo
{
    vmi_instance_t vmi;
    char vmname[50];
    unsigned long vmid;
    char version[50];
    int syscall_len;
    SyscallOne* syscallall;
    OffSet vmoffset[NUMBER_OF_OFFSET];
    int offset_len;
    reg_t syscall;
}VmiInfo;

#define MAX_VM 10
VmiInfo* globalvm[MAX_VM];
int vm_num;

int read_version_conf(VmiInfo* vmiinfo);

int read_syscall_conf(VmiInfo* vmiinfo);

int read_offset_conf(VmiInfo* vmiinfo);

VmiInfo* get_vmiinfo_vmi(vmi_instance_t vmi);

#endif