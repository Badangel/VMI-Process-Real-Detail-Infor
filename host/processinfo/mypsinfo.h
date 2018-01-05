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

extern unsigned long pid_offset, name_offset;

extern unsigned long socket_sk_offset;
extern unsigned long sk_daddr_offset;
extern unsigned long sk_rcv_saddr_offset;
extern unsigned long sk_num_offset;
extern unsigned long sk_dport_offset;
extern unsigned long sk_v6_daddr_offet;
extern unsigned long sk_v6_rcv_saddr_offet;

extern unsigned long fs_offset;
extern unsigned long tgid_offset;
extern unsigned long fsroot_offset;
extern unsigned long fspwd_offset;
extern unsigned long files_offset;
extern unsigned long fdt_offset;
extern unsigned long maxfds_offset;
extern unsigned long nameidata_offset;
extern unsigned long task_real_parent_offset;
extern unsigned long task_parent_offset;
extern unsigned long group_leader_offset;
extern unsigned long start_time_offset;
extern unsigned long real_start_time_offset;
extern unsigned long min_flt_offset;
extern unsigned long maj_flt_offset;
extern unsigned long utime_offset;
extern unsigned long stime_offset;
extern unsigned long gtime_offset;
extern unsigned long ioac_offset;
extern unsigned long prio_offset;
extern unsigned long static_prio_offset;
extern unsigned long normal_prio_offset;
extern unsigned long rt_priority_offset;

extern unsigned long mm_offset;
extern unsigned long pgd_offset;
extern unsigned long mm_users_offset;
extern unsigned long mm_count_offset;
extern unsigned long total_vm_offset;
extern unsigned long locked_vm_offset;
extern unsigned long stack_vm_offset;
extern unsigned long start_code_offset;
extern unsigned long end_code_offset;

extern unsigned long fd_offset;
extern unsigned long i_node_offset;
extern unsigned long i_uid_offset;
extern unsigned long i_gid_offset;
extern unsigned long private_date_offset;
extern unsigned long f_mode_offset;
extern unsigned long f_flags_offset;


extern unsigned long path_offset;
extern unsigned long dentry_offset;
extern unsigned long parent_offset;
extern unsigned long dname_offset;
extern unsigned long qname_offset;
extern unsigned long di_name_offset;


extern unsigned long nameidata_last_offset;
extern unsigned long nameidata_root_offset;
extern unsigned long nameidata_path_offset;

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


/**
 * This funtion get mm_struct info
 */
void get_mm_struct(vmi_instance_t vmi, addr_t currentps, TaskNode *tmptn);

/**
*This funtion get the dentry qstr name.
*/
char* get_dentry_name(vmi_instance_t vmi,char *name,addr_t dentry_addr);

/**
*This funtion for show the relevant files infomations.
*Include files socket and so on.
*/
void get_files_info(vmi_instance_t vmi,addr_t fdaddr,unsigned int max_file, TaskNode *tmp);

/**
*This funtion get the qstr name.
*/
char* get_qstr_name(vmi_instance_t vmi,char *name,addr_t qstr_addr);

/**
*4.4.57 nameidata is null...
*/
void get_nameidata_info(vmi_instance_t vmi, addr_t nameidat);

/**
* show fs information
* only pwd is not null, fs other is null
*/
void get_fs_info(vmi_instance_t vmi, addr_t fs);

void get_task_info(vmi_instance_t vmi,addr_t current_process, TaskNode *tmptn);

void setoffset(VmiInfo* vmiinfo,char* key, unsigned long value);

int getoffset(VmiInfo* vmiinfo,char* key);

#endif // MYPSINFO_H_INCLUDED
