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
    int classify;
}SyscallOne;

typedef struct vmiinfo
{
    vmi_instance_t vmi;
    char vmname[50];
    char version[50];
    int syscall_len;
    SyscallOne* syscallall;
    OffSet vmoffset[NUMBER_OF_OFFSET];
    int offset_len;
}VmiInfo;

int read_syscall_conf(VmiInfo* vmiinfo);

int read_offset_conf(VmiInfo* vmiinfo);

#endif