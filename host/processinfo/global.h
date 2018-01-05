#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>
#include <string.h>
#include <libvmi/libvmi.h>

unsigned long pid_offset = 0, name_offset = 0;

unsigned long socket_sk_offset = 32;
unsigned long sk_daddr_offset = 0;
unsigned long sk_rcv_saddr_offset = 4;
unsigned long sk_num_offset = 14;
unsigned long sk_dport_offset = 12;
unsigned long sk_v6_daddr_offet = 56;
unsigned long sk_v6_rcv_saddr_offet = 72;

unsigned long fs_offset = 1584;
unsigned long tgid_offset = 1100;
unsigned long fsroot_offset = 24;
unsigned long fspwd_offset = 40;
unsigned long files_offset = 1592;
unsigned long fdt_offset = 32;
unsigned long maxfds_offset = 0;
unsigned long nameidata_offset = 1544;
unsigned long task_real_parent_offset = 1112;
unsigned long task_parent_offset = 1120;
unsigned long group_leader_offset = 1160;
unsigned long start_time_offset = 1408;
unsigned long real_start_time_offset = 1416;
unsigned long min_flt_offset = 1424;
unsigned long maj_flt_offset = 1432;
unsigned long utime_offset = 1328;
unsigned long stime_offset = 1336;
unsigned long gtime_offset = 1360;
unsigned long ioac_offset = 1840;
unsigned long prio_offset = 72;
unsigned long static_prio_offset = 76;
unsigned long normal_prio_offset = 80;
unsigned long rt_priority_offset = 84;

unsigned long mm_offset = 928;
unsigned long pgd_offset = 64;
unsigned long mm_users_offset = 72;
unsigned long mm_count_offset = 76;
unsigned long total_vm_offset = 176;
unsigned long locked_vm_offset = 184;
unsigned long stack_vm_offset = 216;
unsigned long start_code_offset = 232;
unsigned long end_code_offset = 240;

unsigned long fd_offset = 8;
unsigned long i_node_offset = 32;
unsigned long i_uid_offset = 4;
unsigned long i_gid_offset = 8;
unsigned long private_date_offset = 208;
unsigned long f_mode_offset = 68;
unsigned long f_flags_offset = 64;


unsigned long path_offset = 16;
unsigned long dentry_offset = 8;
unsigned long parent_offset = 24;
unsigned long dname_offset = 32;
unsigned long qname_offset = 8;
unsigned long di_name_offset = 56;


unsigned long nameidata_last_offset = 16;
unsigned long nameidata_root_offset = 32;
unsigned long nameidata_path_offset = 0;

addr_t file = 1;

addr_t i_node = 1;
addr_t f_path = 1;
addr_t dentry1 = 1;
addr_t d_name = 1;
addr_t f_name = 1;
addr_t p_name = 1;
addr_t d_parent = 1;
addr_t droot = 1;
addr_t mnt = 1;
addr_t rname = 1;
addr_t nameida = 1;
addr_t task_real_parent = 1;
addr_t task_parent = 1;
addr_t group_leader = 1;

addr_t task_fs_root = 1;
addr_t task_fs_pwd = 1;

addr_t fsroot_dentry = 1;
addr_t fspwd_dentry = 1;

addr_t private_date = 1;
addr_t sk = 1;

addr_t fdadd = 8;
addr_t nadd = 1;



uint32_t daddr;
uint32_t rcv_saddr;
uint16_t sk_num;
uint16_t sk_dport;
uint64_t v6_daddr;
uint64_t v6_rcv_saddr;
uint32_t fmode;
uint32_t flags;


char *procname = NULL;
vmi_pid_t pid = 0;
vmi_pid_t real_parent_pid = 0;
vmi_pid_t parent_pid = 0;
vmi_pid_t tgid = 0;
vmi_pid_t group_leader_pid = 0;

status_t status;

addr_t fs = 1;

addr_t files = 1;
addr_t fdt = 1;
addr_t fdtab = 1;
addr_t fd_array = 1;
addr_t fd = 1;

unsigned int max_fds = 0;
char *filename = NULL;
char *d_iname = NULL;
char *parentname = NULL;
#endif