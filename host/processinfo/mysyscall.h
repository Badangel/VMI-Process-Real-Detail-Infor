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
#include "vminit.h"

#define MAX_VM 10
VmiInfo* globalvm[MAX_VM];
int vm_num;

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

#endif // MYSYSCALL_H
