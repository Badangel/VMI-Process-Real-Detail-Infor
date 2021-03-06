/**
* Copyright(c) 2017 dyf
* record and modify syscall enter addr.
* defind pipe transmit syscall format
* interrupt to recover enter addr
*/
#ifndef MYSYSCALL_H_INCLUDED
#define MYSYSCALL_H_INCLUDED

#include <stdint.h>
#include <signal.h>
#include <libvmi/libvmi.h>
#include <libvmi/events.h>
#include <mysql/mysql.h>
#include "myqueue.h"
#include "vminit.h"

extern VmiInfo* globalvm[];
extern int vm_num;

#define NUMBER_OF_SYSCALLS 326

typedef struct persyscall{
    int sysnum;
    uint32_t pid;
}psyscall;


/* Signal handler */
static struct sigaction act;
static int interrupted = 0;
void close_handler(int sig);

int syscallnum[NUMBER_OF_SYSCALLS];
uint8_t trap;
reg_t rdi, rax,cr3;
int trapnum;//will delete
int singstepnum;//will delete
int sys_num;
int pipenum;
///extern int writen;

event_response_t singlestep_cb(vmi_instance_t vmi, vmi_event_t *event);

event_response_t trap_cb(vmi_instance_t vmi, vmi_event_t *event);

void combineSyscallAndPs(LinkQueue* queue,int sysnum[][11],int psnum);

void find_syscall_hook(VmiInfo* vmivm,MYSQL* mysql,int i,uint64_t backup_byte);

void record_syscall(VmiInfo* vmivm, reg_t rax,vmi_pid_t pid,char* psname,addr_t currentpsaddr,vmi_event_t *event);

void get_module_name(VmiInfo* vmivm,char* unlink_path);

void detect_syscall_hook(VmiInfo* vmivm);

int get_sysnum_from_rip(VmiInfo* vmivm,uint64_t rip);

#endif // MYSYSCALL_H
