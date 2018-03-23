/**
* Copyright(c) 2017 dyf
* mypsinfo.h
* record task_struct/files/fd/sk/dentry... offset
* give some funtions to get infomation about one task deeply
*/

#ifndef MYPSINFO_H_INCLUDED
#define MYPSINFO_H_INCLUDED

#include <stdint.h>
#include <string.h>
#include <libvmi/libvmi.h>
#include "myqueue.h"
#include "vminit.h"

/*
extern addr_t file;

extern addr_t i_node;
extern addr_t f_path;
extern addr_t dentry1;
extern addr_t d_name;
extern addr_t f_name;
extern addr_t p_name;
extern addr_t d_parent;
extern addr_t droot;
extern addr_t mnt;
extern addr_t rname;
extern addr_t nameida;
extern addr_t task_real_parent;
extern addr_t task_parent;
extern addr_t group_leader;

extern addr_t task_fs_root;
extern addr_t task_fs_pwd;

extern addr_t fsroot_dentry;
extern addr_t fspwd_dentry;

extern addr_t private_date;
extern addr_t sk;

extern addr_t fdadd;
extern addr_t nadd;

extern addr_t fs;

extern addr_t files;
extern addr_t fdt;
extern addr_t fdtab;
extern addr_t fd_array;
extern addr_t fd;


uint32_t daddr;
uint32_t rcv_saddr;
uint16_t sk_num;
uint16_t sk_dport;
uint64_t v6_daddr;
uint64_t v6_rcv_saddr;
uint32_t fmode;
uint32_t flags;


extern char *procname;
extern  vmi_pid_t pid;
extern  vmi_pid_t real_parent_pid ;
extern  vmi_pid_t parent_pid;
extern  vmi_pid_t tgid;
extern  vmi_pid_t group_leader_pid;

status_t status;


extern unsigned int max_fds;
extern char *filename;
extern char *d_iname;
extern char *parentname;
*/

/**
 * This funtion get mm_struct info
 */
void get_mm_struct(VmiInfo* vmiinfo, addr_t currentps, TaskNode *tmptn);

/**
*This funtion get the dentry qstr name.
*/
char* get_dentry_name(VmiInfo* vmiinfo,char *name,addr_t dentry_addr);

/**
*This funtion for show the relevant files infomations.
*Include files socket and so on.
*/
void get_files_info(VmiInfo* vmiinfo,addr_t fdaddr,unsigned int max_file, TaskNode *tmp);

/**
*This funtion get the qstr name.
*/
char* get_qstr_name(VmiInfo* vmiinfo,char *name,addr_t qstr_addr);

/**
*4.4.57 nameidata is null...
*/
void get_nameidata_info(VmiInfo* vmiinfo, addr_t nameidat);

/**
* show fs information
* only pwd is not null, fs other is null
*/
void get_fs_info(VmiInfo* vmiinfo, addr_t fs);

void get_task_info(VmiInfo* vmiinfo,addr_t current_process, TaskNode *tmptn);

addr_t get_psname_by_pid(VmiInfo* vmiinfo,vmi_pid_t pid,char* psname);

void get_psname_by_addr(VmiInfo* vmiinfo,char* psname,addr_t current_process);

//void setoffset(VmiInfo* vmiinfo,char* key, unsigned long value);

//int getoffset(VmiInfo* vmiinfo,char* key);
void initPs(VmiInfo* vmiinfo,addr_t list_head);

PsNode* get_ps_fron_pgd(VmiInfo* vmiinfo,addr_t nowpgd);

PsNode* get_ps_fron_pid(VmiInfo* vmiinfo,vmi_pid_t pid);

void delete_one_ps(VmiInfo* vmiinfo,vmi_pid_t pid);

int compare2ps(void* a, void* b);
int compare2ps_nameid(void* a, void* b);

void add_pslist(VmiInfo* vmiinfo,PsNode* psnode);

void record_ps_list(VmiInfo* vmiinfo);

void clear_ps_file(VmiInfo* vmiinfo);

void read_pslist_from_file(VmiInfo* vmiinfo,MyList *pslist_real);

void detect_hide_ps(VmiInfo* vmiinfo,MYSQL *mysql,LinkQueue* queue, MyList *pslist_real);

#endif // MYPSINFO_H_INCLUDED
