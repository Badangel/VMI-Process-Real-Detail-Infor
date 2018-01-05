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
#include "myqueue.h"


#define NUMBER_OF_SYSCALLS 326

typedef struct persyscall{
    int sysnum;
    uint32_t pid;
}psyscall;


typedef struct syscall
{
    int num;
    uint64_t addr;
    char name[30];
    uint8_t pre;
    int classify;
}Syscall;



/* Signal handler */
static struct sigaction act;
static int interrupted = 0;
void close_handler(int sig);

///(1)ps_control (2)file_rw (3)file_control (4)sys_control (5)mem_control (6)net_control (7)socket_control (8)user_control (9)ps_communcation
extern Syscall syscalls[NUMBER_OF_SYSCALLS];

extern int syscallnum[NUMBER_OF_SYSCALLS];
extern uint8_t trap;
extern reg_t rdi, rax,cr3;
extern int trapnum;
extern int singstepnum;
extern int sys_num;
extern int pipenum;
extern int writen;

event_response_t singlestep_cb(vmi_instance_t vmi, vmi_event_t *event);

event_response_t trap_cb(vmi_instance_t vmi, vmi_event_t *event);

void combineSyscallAndPs(LinkQueue* queue,int sysnum[][11],int psnum);

#endif // MYSYSCALL_H
