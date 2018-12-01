/**
* Copyright(c) 2018 dyf
* .h vm read syscall config
*/

#ifndef VMINIT_H_INCLUDED
#define VMINIT_H_INCLUDED

#include <libvmi/libvmi.h>
#include <stdio.h>
#include "myList.h"

#define NUMBER_OF_OFFSET 60

typedef struct TaskNode{
    uint32_t tspid;
    unsigned int tsfdnum;
    //socket in UNIX,NETLINK,TCP,UDP,TCPv6,other
    //FdInfo tsfdinfo;
    int socketinfo[6];
    //anon_inode in [eventfd],inotify,[timerfd],[signalfd],[eventpoll],other
    int anon_inodeinfo[6];
    int pipeinfo;
    int nullinfo;
    int numinfo;
    int fileinfo;

    uint32_t tsparent;
    int tslayer;
    uint32_t tsgroupleader;
    int tsprio;
    unsigned int tsutime;
    unsigned int tsstime;
    unsigned int tsinc_utime;
    unsigned int tsinc_stime;

    uint64_t tsstart_time;
    uint64_t tsrealstart_time;
    unsigned long tsminflt;
    unsigned long tsmajflt;
    unsigned long tsinc_minflt;
    unsigned long tsinc_majflt;

    uint64_t tsrchar;
    uint64_t tswchar;
    uint64_t tssyscr;
    uint64_t tssyscw;
    uint64_t tsread_bytes;
	uint64_t tswrite_bytes;
	uint64_t tscancelled_write_bytes;

    uint64_t tsinc_rchar;
    uint64_t tsinc_wchar;
    uint64_t tsinc_syscr;
    uint64_t tsinc_syscw;
    uint64_t tsinc_read_bytes;
	uint64_t tsinc_write_bytes;
	uint64_t tsinc_cancelled_write_bytes;

    uint32_t tsmm_users;
    uint32_t tsmm_count;
    unsigned long tstotal_vm;
    unsigned long tsstack_vm;

    int syscallnum[11];
    MyList* socket_list;
    MyList* file_list;

    char tsname[255];
    int state;
    struct TaskNode* next;

}TaskNode;

typedef struct modulenode
{
    char name[50];
    int num_syms;
    int num_kp;
}ModuleNode;

typedef struct psnode
{
    char name[50];
    uint64_t addr;
    uint32_t pid;
    uint64_t pgd;
}PsNode;

typedef struct socketsr
{
    int classify;
    ///0:don't know 1:tcp 2:udp
    char sendaddr[50];
    uint16_t sendport;
    char recvaddr[50];
    uint16_t recieveport;
}SocketSR;

typedef struct filefd
{
    ///0:don't know 1:tcp 2:udp
    int fd;
    char filepath[255];
}FileFD;

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
    uint64_t reallyaddr;
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
    MyList* acl_list;
    //OffSet vmoffset[NUMBER_OF_OFFSET];
    int offset_len;
    char modl[100];
    int module_num;
    MyList* modulelist;
    char pslistfile[100];
    int ps_num;
    MyList* pslist;
    char exitpsfile[100];
    char sklistfile[100];
    unsigned long vmoffset[NUMBER_OF_OFFSET];
    reg_t syscall;
}VmiInfo;

enum
{
    task_offset,pid_offset, name_offset, socket_sk_offset, sk_daddr_offset, sk_rcv_saddr_offset, sk_num_offset, sk_dport_offset, sk_v6_daddr_offset, sk_v6_rcv_saddr_offset, fs_offset, tgid_offset, fsroot_offset, fspwd_offset, files_offset, fdt_offset, maxfds_offset, nameidata_offset, task_real_parent_offset, task_parent_offset, group_leader_offset, start_time_offset, real_start_time_offset, min_flt_offset, maj_flt_offset, utime_offset, stime_offset, gtime_offset, ioac_offset, prio_offset, static_prio_offset, normal_prio_offset, rt_priority_offset, mm_offset, pgd_offset, mm_users_offset, mm_count_offset, total_vm_offset, locked_vm_offset, stack_vm_offset, start_code_offset, end_code_offset, fd_offset, i_node_offset, i_uid_offset, i_gid_offset, private_date_offset, f_mode_offset, f_flags_offset, path_offset, dentry_offset, parent_offset, dname_offset, qname_offset, di_name_offset, nameidata_last_offset, nameidata_root_offset, nameidata_path_offset, source_list_offset, target_list_offset
}VmOffset;

#define MAX_VM 10
VmiInfo* globalvm[MAX_VM];
int vm_num;

int read_version_conf(VmiInfo* vmiinfo);

int read_syscall_conf(VmiInfo* vmiinfo);

int read_offset_conf(VmiInfo* vmiinfo);

VmiInfo* get_vmiinfo_vmi(vmi_instance_t vmi);

int set_offset(VmiInfo* vmiinfo,char* offsetname,unsigned long value);

int get_file_string(vmi_instance_t vmi,addr_t startaddr, pid_t pid,char* mystring);

#endif