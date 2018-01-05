/**
* Copyright(c) 2017 dyf
* record and modify syscall enter addr.
* defind pipe transmit syscall format
* interrupt to recover enter addr
*/

#include <signal.h>
#include "mysyscall.h"


/* Signal handler */
void close_handler(int sig)
{
    interrupted = sig;
}
int syscallnum[NUMBER_OF_SYSCALLS] = {0};
Syscall syscalls[NUMBER_OF_SYSCALLS] =
{
    {.num = 0, .addr = 0x0000000000000000, .name = "sys_read", .pre = 0x00, .classify = 2},//0xffffffff8120f4f0
    {.num = 1, .addr = 0x0000000000000000, .name = "sys_write", .pre = 0x00, .classify = 2},//0xffffffff8120f5b0
    {.num = 2, .addr = 0xffffffff8120d9c0, .name = "sys_open", .pre = 0x00, .classify = 2},
    {.num = 3, .addr = 0x0000000000000000, .name = "sys_close", .pre = 0x00, .classify = 2},//0xffffffff8120b8a0 can't sign in
    {.num = 4, .addr = 0xffffffff81213d40, .name = "sys_stat", .pre = 0x00, .classify = 3},
    {.num = 5, .addr = 0xffffffff81213d60, .name = "sys_fstat", .pre = 0x00, .classify = 3},
    {.num = 6, .addr = 0xffffffff81213d50, .name = "sys_lstat", .pre = 0x00, .classify = 3},
    {.num = 7, .addr = 0x0000000000000000, .name = "sys_poll", .pre = 0x00, .classify = 2},//0xffffffff81224590
    {.num = 8, .addr = 0xffffffff8120e400, .name = "sys_lseek", .pre = 0x00, .classify = 2},
    {.num = 9, .addr = 0xffffffff81034050, .name = "sys_mmap", .pre = 0x00, .classify = 5},
    {.num = 10, .addr = 0x0000000000000000, .name = "sys_mprotect", .pre = 0x00, .classify = 5},//0xffffffff811cab50
    {.num = 11, .addr = 0xffffffff811c94a0, .name = "sys_munmap", .pre = 0x00, .classify = 5},
    {.num = 12, .addr = 0xffffffff811c88a0, .name = "sys_brk", .pre = 0x00, .classify = 5},
    {.num = 13, .addr = 0xffffffff81092400, .name = "sys_rt_sigaction", .pre = 0x00, .classify = 9},
    {.num = 14, .addr = 0xffffffff81091180, .name = "sys_rt_sigprocmask", .pre = 0x00, .classify = 9},
    {.num = 15, .addr = 0xffffffff8102ed00, .name = "sys_rt_sigreturn", .pre = 0x00, .classify = 9},
    {.num = 16, .addr = 0xffffffff81222440, .name = "sys_ioctl", .pre = 0x00, .classify = 4},
    {.num = 17, .addr = 0xffffffff8120f670, .name = "sys_pread64", .pre = 0x00, .classify = 2},
    {.num = 18, .addr = 0xffffffff8120f720, .name = "sys_pwrite64", .pre = 0x00, .classify = 2},
    {.num = 19, .addr = 0xffffffff8120f8e0, .name = "sys_readv", .pre = 0x00, .classify = 2},
    {.num = 20, .addr = 0x0000000000000000, .name = "sys_writev", .pre = 0x00, .classify = 2},//0xffffffff8120f9d0
    {.num = 21, .addr = 0xffffffff8120ca20, .name = "sys_access", .pre = 0x00, .classify = 3},
    {.num = 22, .addr = 0xffffffff81218000, .name = "sys_pipe", .pre = 0x00, .classify = 9},
    {.num = 23, .addr = 0x0000000000000000, .name = "sys_select", .pre = 0x00, .classify = 7},//0xffffffff81223c70
    {.num = 24, .addr = 0xffffffff810ae9e0, .name = "sys_sched_yield", .pre = 0x00, .classify = 1},
    {.num = 25, .addr = 0xffffffff811cb940, .name = "sys_mremap", .pre = 0x00, .classify = 5},
    {.num = 26, .addr = 0xffffffff811cbe30, .name = "sys_msync", .pre = 0x00, .classify = 5},
    {.num = 27, .addr = 0xffffffff811c45a0, .name = "sys_mincore", .pre = 0x00, .classify = 5},
    {.num = 28, .addr = 0xffffffff811d3330, .name = "sys_madvise", .pre = 0x00, .classify = 5},
    {.num = 29, .addr = 0xffffffff81334770, .name = "sys_shmget", .pre = 0x00, .classify = 9},
    {.num = 30, .addr = 0xffffffff81334f50, .name = "sys_shmat", .pre = 0x00, .classify = 9},
    {.num = 31, .addr = 0xffffffff813347e0, .name = "sys_shmctl", .pre = 0x00, .classify = 9},
    {.num = 32, .addr = 0xffffffff8122d140, .name = "sys_dup", .pre = 0x00, .classify = 2},
    {.num = 33, .addr = 0xffffffff8122d030, .name = "sys_dup2", .pre = 0x00, .classify = 2},
    {.num = 34, .addr = 0xffffffff81092750, .name = "sys_pause", .pre = 0x00, .classify = 1},
    {.num = 35, .addr = 0xffffffff810f0a30, .name = "sys_nanosleep", .pre = 0x00, .classify = 1},
    {.num = 36, .addr = 0xffffffff810f0f10, .name = "sys_getitimer", .pre = 0x00, .classify = 4},
    {.num = 37, .addr = 0xffffffff810ef380, .name = "sys_alarm", .pre = 0x00, .classify = 4},
    {.num = 38, .addr = 0x0000000000000000, .name = "sys_setitimer", .pre = 0x00, .classify = 4},//0xffffffff810f12e0
    {.num = 39, .addr = 0xffffffff81094e70, .name = "sys_getpid", .pre = 0x00, .classify = 1},
    {.num = 40, .addr = 0xffffffff8120fe60, .name = "sys_sendfile", .pre = 0x00, .classify = 7},
    {.num = 41, .addr = 0xffffffff8170f7c0, .name = "sys_socket", .pre = 0x00, .classify = 7},
    {.num = 42, .addr = 0xffffffff8170fbb0, .name = "sys_connect", .pre = 0x00, .classify = 7},
    {.num = 43, .addr = 0xffffffff8170fb90, .name = "sys_accept", .pre = 0x00, .classify = 7},
    {.num = 44, .addr = 0xffffffff8170fbe0, .name = "sys_sendto", .pre = 0x00, .classify = 7},
    {.num = 45, .addr = 0xffffffff8170fc10, .name = "sys_recvfrom", .pre = 0x00, .classify = 7},
    {.num = 46, .addr = 0xffffffff8170ff20, .name = "sys_sendmsg", .pre = 0x00, .classify = 7},
    {.num = 47, .addr = 0x0000000000000000, .name = "sys_recvmsg", .pre = 0x00, .classify = 7},//0xffffffff81710180
    {.num = 48, .addr = 0xffffffff8170fe00, .name = "sys_shutdown", .pre = 0x00, .classify = 7},
    {.num = 49, .addr = 0xffffffff8170fac0, .name = "sys_bind", .pre = 0x00, .classify = 7},
    {.num = 50, .addr = 0xffffffff8170fad0, .name = "sys_listen", .pre = 0x00, .classify = 7},
    {.num = 51, .addr = 0xffffffff8170fbc0, .name = "sys_getsockname", .pre = 0x00, .classify = 7},
    {.num = 52, .addr = 0xffffffff8170fbd0, .name = "sys_getpeername", .pre = 0x00, .classify = 7},
    {.num = 53, .addr = 0xffffffff8170f870, .name = "sys_socketpair", .pre = 0x00, .classify = 7},
    {.num = 54, .addr = 0xffffffff8170fc40, .name = "sys_setsockopt", .pre = 0x00, .classify = 7},
    {.num = 55, .addr = 0xffffffff8170fd20, .name = "sys_getsockopt", .pre = 0x00, .classify = 7},
    {.num = 56, .addr = 0xffffffff81080910, .name = "sys_clone", .pre = 0x00, .classify = 1},
    {.num = 57, .addr = 0xffffffff810808b0, .name = "sys_fork", .pre = 0x00, .classify = 1},
    {.num = 58, .addr = 0xffffffff810808e0, .name = "sys_vfork", .pre = 0x00, .classify = 1},
    {.num = 59, .addr = 0xffffffff81216840, .name = "sys_execve", .pre = 0x00, .classify = 1},
    {.num = 60, .addr = 0xffffffff810847d0, .name = "sys_exit", .pre = 0x00, .classify = 1},
    {.num = 61, .addr = 0xffffffff81084a70, .name = "sys_wait4", .pre = 0x00, .classify = 1},
    {.num = 62, .addr = 0xffffffff81091a50, .name = "sys_kill", .pre = 0x00, .classify = 9},
    {.num = 63, .addr = 0xffffffff81095590, .name = "sys_uname", .pre = 0x00, .classify = 4},
    {.num = 64, .addr = 0xffffffff81332d60, .name = "sys_semget", .pre = 0x00, .classify = 9},
    {.num = 65, .addr = 0xffffffff813330a0, .name = "sys_semop", .pre = 0x00, .classify = 9},
    {.num = 66, .addr = 0xffffffff81332de0, .name = "sys_semctl", .pre = 0x00, .classify = 9},
    {.num = 67, .addr = 0xffffffff81334fa0, .name = "sys_shmdt", .pre = 0x00, .classify = 9},
    {.num = 68, .addr = 0xffffffff8132f640, .name = "sys_msgget", .pre = 0x00, .classify = 9},
    {.num = 69, .addr = 0xffffffff8132fb10, .name = "sys_msgsnd", .pre = 0x00, .classify = 9},
    {.num = 70, .addr = 0xffffffff813300f0, .name = "sys_msgrcv", .pre = 0x00, .classify = 9},
    {.num = 71, .addr = 0xffffffff8132f6b0, .name = "sys_msgctl", .pre = 0x00, .classify = 9},
    {.num = 72, .addr = 0xffffffff81220f60, .name = "sys_fcntl", .pre = 0x00, .classify = 2},
    {.num = 73, .addr = 0xffffffff81264ba0, .name = "sys_flock", .pre = 0x00, .classify = 2},
    {.num = 74, .addr = 0xffffffff81242c40, .name = "sys_fsync", .pre = 0x00, .classify = 2},
    {.num = 75, .addr = 0xffffffff81242c60, .name = "sys_fdatasync", .pre = 0x00, .classify = 5},
    {.num = 76, .addr = 0xffffffff8120c720, .name = "sys_truncate", .pre = 0x00, .classify = 2},
    {.num = 77, .addr = 0xffffffff8120c750, .name = "sys_ftruncate", .pre = 0x00, .classify = 2},
    {.num = 78, .addr = 0xffffffff81222970, .name = "sys_getdents", .pre = 0x00, .classify = 3},
    {.num = 79, .addr = 0xffffffff812284f0, .name = "sys_getcwd", .pre = 0x00, .classify = 3},
    {.num = 80, .addr = 0xffffffff8120cc50, .name = "sys_chdir", .pre = 0x00, .classify = 3},
    {.num = 81, .addr = 0xffffffff8120cd20, .name = "sys_fchdir", .pre = 0x00, .classify = 3},
    {.num = 82, .addr = 0xffffffff812208a0, .name = "sys_rename", .pre = 0x00, .classify = 3},
    {.num = 83, .addr = 0xffffffff8121f6d0, .name = "sys_mkdir", .pre = 0x00, .classify = 3},
    {.num = 84, .addr = 0xffffffff8121f7d0, .name = "sys_rmdir", .pre = 0x00, .classify = 3},
    {.num = 85, .addr = 0xffffffff8120da00, .name = "sys_creat", .pre = 0x00, .classify = 2},
    {.num = 86, .addr = 0xffffffff8121fcd0, .name = "sys_link", .pre = 0x00, .classify = 3},
    {.num = 87, .addr = 0xffffffff8121f820, .name = "sys_unlink", .pre = 0x00, .classify = 3},
    {.num = 88, .addr = 0xffffffff8121f950, .name = "sys_symlink", .pre = 0x00, .classify = 3},
    {.num = 89, .addr = 0xffffffff81213ed0, .name = "sys_readlink", .pre = 0x00, .classify = 3},
    {.num = 90, .addr = 0xffffffff8120d010, .name = "sys_chmod", .pre = 0x00, .classify = 3},
    {.num = 91, .addr = 0xffffffff8120cee0, .name = "sys_fchmod", .pre = 0x00, .classify = 3},
    {.num = 92, .addr = 0xffffffff8120d1d0, .name = "sys_chown", .pre = 0x00, .classify = 3},
    {.num = 93, .addr = 0xffffffff8120d390, .name = "sys_fchown", .pre = 0x00, .classify = 3},
    {.num = 94, .addr = 0xffffffff8120d2b0, .name = "sys_lchown", .pre = 0x00, .classify = 3},
    {.num = 95, .addr = 0xffffffff81096000, .name = "sys_umask", .pre = 0x00, .classify = 2},
    {.num = 96, .addr = 0x0000000000000000, .name = "sys_gettimeofday", .pre = 0x00, .classify = 4},//0xffffffff810ebce0
    {.num = 97, .addr = 0xffffffff81095c40, .name = "sys_getrlimit", .pre = 0x00, .classify = 4},
    {.num = 98, .addr = 0xffffffff81095fb0, .name = "sys_getrusage", .pre = 0x00, .classify = 4},
    {.num = 99, .addr = 0xffffffff81096520, .name = "sys_sysinfo", .pre = 0x00, .classify = 4},
    {.num = 100, .addr = 0xffffffff81095060, .name = "sys_times", .pre = 0x00, .classify = 4},
    {.num = 101, .addr = 0xffffffff8108c730, .name = "sys_ptrace", .pre = 0x00, .classify = 1},
    {.num = 102, .addr = 0xffffffff81094ef0, .name = "sys_getuid", .pre = 0x00, .classify = 8},
    {.num = 103, .addr = 0xffffffff810da250, .name = "sys_syslog", .pre = 0x00, .classify = 0},
    {.num = 104, .addr = 0xffffffff81094f50, .name = "sys_getgid", .pre = 0x00, .classify = 8},
    {.num = 105, .addr = 0xffffffff81094700, .name = "sys_setuid", .pre = 0x00, .classify = 8},
    {.num = 106, .addr = 0xffffffff81094440, .name = "sys_setgid", .pre = 0x00, .classify = 8},
    {.num = 107, .addr = 0xffffffff81094f20, .name = "sys_geteuid", .pre = 0x00, .classify = 8},
    {.num = 108, .addr = 0xffffffff81094f80, .name = "sys_getegid", .pre = 0x00, .classify = 8},
    {.num = 109, .addr = 0xffffffff810950e0, .name = "sys_setpgid", .pre = 0x00, .classify = 1},
    {.num = 110, .addr = 0xffffffff81094ec0, .name = "sys_getppid", .pre = 0x00, .classify = 1},
    {.num = 111, .addr = 0xffffffff81095310, .name = "sys_getpgrp", .pre = 0x00, .classify = 1},
    {.num = 112, .addr = 0xffffffff810953b0, .name = "sys_setsid", .pre = 0x00, .classify = 1},
    {.num = 113, .addr = 0xffffffff81094500, .name = "sys_setreuid", .pre = 0x00, .classify = 8},
    {.num = 114, .addr = 0xffffffff810942d0, .name = "sys_setregid", .pre = 0x00, .classify = 8},
    {.num = 115, .addr = 0xffffffff810a4d70, .name = "sys_getgroups", .pre = 0x00, .classify = 8},
    {.num = 116, .addr = 0xffffffff810a4e80, .name = "sys_setgroups", .pre = 0x00, .classify = 8},
    {.num = 117, .addr = 0xffffffff81094800, .name = "sys_setresuid", .pre = 0x00, .classify = 8},
    {.num = 118, .addr = 0xffffffff810949f0, .name = "sys_getresuid", .pre = 0x00, .classify = 8},
    {.num = 119, .addr = 0xffffffff81094ab0, .name = "sys_setresgid", .pre = 0x00, .classify = 8},
    {.num = 120, .addr = 0xffffffff81094c40, .name = "sys_getresgid", .pre = 0x00, .classify = 8},
    {.num = 121, .addr = 0xffffffff810952a0, .name = "sys_getpgid", .pre = 0x00, .classify = 1},
    {.num = 122, .addr = 0xffffffff81094ce0, .name = "sys_setfsuid", .pre = 0x00, .classify = 8},
    {.num = 123, .addr = 0xffffffff81094db0, .name = "sys_setfsgid", .pre = 0x00, .classify = 8},
    {.num = 124, .addr = 0xffffffff81095340, .name = "sys_getsid", .pre = 0x00, .classify = 1},
    {.num = 125, .addr = 0xffffffff8108a970, .name = "sys_capget", .pre = 0x00, .classify = 1},
    {.num = 126, .addr = 0xffffffff8108aae0, .name = "sys_capset", .pre = 0x00, .classify = 1},
    {.num = 127, .addr = 0xffffffff81091300, .name = "sys_rt_sigpending", .pre = 0x00, .classify = 9},
    {.num = 128, .addr = 0xffffffff81091a40, .name = "sys_rt_sigtimedwait", .pre = 0x00, .classify = 9},
    {.num = 129, .addr = 0xffffffff81091ac0, .name = "sys_rt_sigqueueinfo", .pre = 0x00, .classify = 9},
    {.num = 130, .addr = 0xffffffff810927a0, .name = "sys_rt_sigsuspend", .pre = 0x00, .classify = 9},
    {.num = 131, .addr = 0xffffffff81091e40, .name = "sys_sigaltstack", .pre = 0x00, .classify = 9},
    {.num = 132, .addr = 0xffffffff81243250, .name = "sys_utime", .pre = 0x00, .classify = 3},
    {.num = 133, .addr = 0xffffffff8121f3d0, .name = "sys_mknod", .pre = 0x00, .classify = 3},
    {.num = 134, .addr = 0xffffffff81215c40, .name = "sys_uselib", .pre = 0x00, .classify = 4},
    {.num = 135, .addr = 0xffffffff81080ed0, .name = "sys_personality", .pre = 0x00, .classify = 1},
    {.num = 136, .addr = 0xffffffff812441e0, .name = "sys_ustat", .pre = 0x00, .classify = 3},
    {.num = 137, .addr = 0xffffffff81244080, .name = "sys_statfs", .pre = 0x00, .classify = 3},
    {.num = 138, .addr = 0xffffffff812440a0, .name = "sys_fstatfs", .pre = 0x00, .classify = 3},
    {.num = 139, .addr = 0xffffffff8122d590, .name = "sys_sysfs", .pre = 0x00, .classify = 4},
    {.num = 140, .addr = 0xffffffff810940a0, .name = "sys_getpriority", .pre = 0x00, .classify = 1},
    {.num = 141, .addr = 0xffffffff81093e50, .name = "sys_setpriority", .pre = 0x00, .classify = 1},
    {.num = 142, .addr = 0xffffffff810ae170, .name = "sys_sched_setparam", .pre = 0x00, .classify = 1},
    {.num = 143, .addr = 0xffffffff810ae380, .name = "sys_sched_getparam", .pre = 0x00, .classify = 1},
    {.num = 144, .addr = 0xffffffff810ae150, .name = "sys_sched_setscheduler", .pre = 0x00, .classify = 1},
    {.num = 145, .addr = 0xffffffff810ae320, .name = "sys_sched_getscheduler", .pre = 0x00, .classify = 1},
    {.num = 146, .addr = 0xffffffff810aea40, .name = "sys_sched_get_priority_max", .pre = 0x00, .classify = 1},
    {.num = 147, .addr = 0xffffffff810aea80, .name = "sys_sched_get_priority_min", .pre = 0x00, .classify = 1},
    {.num = 148, .addr = 0xffffffff810aeac0, .name = "sys_sched_rr_get_interval", .pre = 0x00, .classify = 1},
    {.num = 149, .addr = 0xffffffff811c56c0, .name = "sys_mlock", .pre = 0x00, .classify = 5},
    {.num = 150, .addr = 0xffffffff811c5720, .name = "sys_munlock", .pre = 0x00, .classify = 5},
    {.num = 151, .addr = 0xffffffff811c57a0, .name = "sys_mlockall", .pre = 0x00, .classify = 5},
    {.num = 152, .addr = 0xffffffff811c5950, .name = "sys_munlockall", .pre = 0x00, .classify = 5},
    {.num = 153, .addr = 0xffffffff8120da20, .name = "sys_vhangup", .pre = 0x00, .classify = 4},
    {.num = 154, .addr = 0xffffffff81033020, .name = "sys_modify_ldt", .pre = 0x00, .classify = 1},
    {.num = 155, .addr = 0xffffffff81232130, .name = "sys_pivot_root", .pre = 0x00, .classify = 0},
    {.num = 156, .addr = 0xffffffff8108a530, .name = "sys_sysctl", .pre = 0x00, .classify = 4},
    {.num = 157, .addr = 0xffffffff81096030, .name = "sys_prctl", .pre = 0x00, .classify = 1},
    {.num = 158, .addr = 0xffffffff8102e0b0, .name = "sys_arch_prctl", .pre = 0x00, .classify = 1},
    {.num = 159, .addr = 0xffffffff810ebf60, .name = "sys_adjtimex", .pre = 0x00, .classify = 4},
    {.num = 160, .addr = 0xffffffff81095ec0, .name = "sys_setrlimit", .pre = 0x00, .classify = 4},
    {.num = 161, .addr = 0xffffffff8120cdc0, .name = "sys_chroot", .pre = 0x00, .classify = 3},
    {.num = 162, .addr = 0xffffffff81242ac0, .name = "sys_sync", .pre = 0x00, .classify = 5},
    {.num = 163, .addr = 0xffffffff8110c9f0, .name = "sys_acct", .pre = 0x00, .classify = 4},
    {.num = 164, .addr = 0xffffffff810ebe70, .name = "sys_settimeofday", .pre = 0x00, .classify = 4},
    {.num = 165, .addr = 0xffffffff81231f80, .name = "sys_mount", .pre = 0x00, .classify = 3},
    {.num = 166, .addr = 0x0000000000000000, .name = "sys_umount2", .pre = 0x00, .classify = 3},
    {.num = 167, .addr = 0xffffffff811d7e40, .name = "sys_swapon", .pre = 0x00, .classify = 4},
    {.num = 168, .addr = 0xffffffff811d7840, .name = "sys_swapoff", .pre = 0x00, .classify = 4},
    {.num = 169, .addr = 0xffffffff810a3500, .name = "sys_reboot", .pre = 0x00, .classify = 4},
    {.num = 170, .addr = 0xffffffff81095680, .name = "sys_sethostname", .pre = 0x00, .classify = 6},
    {.num = 171, .addr = 0xffffffff81095820, .name = "sys_setdomainname", .pre = 0x00, .classify = 6},
    {.num = 172, .addr = 0xffffffff810319f0, .name = "sys_iopl", .pre = 0x00, .classify = 4},
    {.num = 173, .addr = 0xffffffff81031870, .name = "sys_ioperm", .pre = 0x00, .classify = 4},
    {.num = 174, .addr = 0x0000000000000000, .name = "sys_create_module", .pre = 0x00, .classify = 4},
    {.num = 175, .addr = 0xffffffff8110b1a0, .name = "sys_init_module", .pre = 0x00, .classify = 4},
    {.num = 176, .addr = 0xffffffff81108430, .name = "sys_delete_module", .pre = 0x00, .classify = 4},
    {.num = 177, .addr = 0x0000000000000000, .name = "sys_get_kernel_syms", .pre = 0x00, .classify = 4},
    {.num = 178, .addr = 0x0000000000000000, .name = "sys_query_module", .pre = 0x00, .classify = 4},
    {.num = 179, .addr = 0xffffffff81277fb0, .name = "sys_quotactl", .pre = 0x00, .classify = 3},
    {.num = 180, .addr = 0x0000000000000000, .name = "sys_nfsservctl", .pre = 0x00, .classify = 4},
    {.num = 181, .addr = 0x0000000000000000, .name = "sys_getpmsg", .pre = 0x00, .classify = 0},
    {.num = 182, .addr = 0x0000000000000000, .name = "sys_putpmsg", .pre = 0x00, .classify = 0},
    {.num = 183, .addr = 0x0000000000000000, .name = "sys_afs_syscall", .pre = 0x00, .classify = 0},
    {.num = 184, .addr = 0x0000000000000000, .name = "sys_tuxcall", .pre = 0x00, .classify = 0},
    {.num = 185, .addr = 0x0000000000000000, .name = "sys_security", .pre = 0x00, .classify = 0},
    {.num = 186, .addr = 0xffffffff81094ea0, .name = "sys_gettid", .pre = 0x00, .classify = 1},
    {.num = 187, .addr = 0xffffffff8119de00, .name = "sys_readahead", .pre = 0x00, .classify = 5},
    {.num = 188, .addr = 0xffffffff81235150, .name = "sys_setxattr", .pre = 0x00, .classify = 3},
    {.num = 189, .addr = 0xffffffff81235170, .name = "sys_lsetxattr", .pre = 0x00, .classify = 3},
    {.num = 190, .addr = 0xffffffff81235190, .name = "sys_fsetxattr", .pre = 0x00, .classify = 3},
    {.num = 191, .addr = 0xffffffff81235260, .name = "sys_getxattr", .pre = 0x00, .classify = 3},
    {.num = 192, .addr = 0xffffffff81235280, .name = "sys_lgetxattr", .pre = 0x00, .classify = 3},
    {.num = 193, .addr = 0xffffffff812352a0, .name = "sys_fgetxattr", .pre = 0x00, .classify = 3},
    {.num = 194, .addr = 0xffffffff81235330, .name = "sys_listxattr", .pre = 0x00, .classify = 3},
    {.num = 195, .addr = 0xffffffff81235350, .name = "sys_llistxattr", .pre = 0x00, .classify = 3},
    {.num = 196, .addr = 0xffffffff81235370, .name = "sys_flistxattr", .pre = 0x00, .classify = 3},
    {.num = 197, .addr = 0xffffffff81235400, .name = "sys_removexattr", .pre = 0x00, .classify = 3},
    {.num = 198, .addr = 0xffffffff81235420, .name = "sys_lremovexattr", .pre = 0x00, .classify = 3},
    {.num = 199, .addr = 0xffffffff81235440, .name = "sys_fremovexattr", .pre = 0x00, .classify = 3},
    {.num = 200, .addr = 0xffffffff81091a90, .name = "sys_tkill", .pre = 0x00, .classify = 9},
    {.num = 201, .addr = 0xffffffff810ebc20, .name = "sys_time", .pre = 0x00, .classify = 4},
    {.num = 202, .addr = 0x0000000000000000, .name = "sys_futex", .pre = 0x00, .classify = 9},//0xffffffff81103b80 get into error
    {.num = 203, .addr = 0xffffffff810ae810, .name = "sys_sched_setaffinity", .pre = 0x00, .classify = 4},
    {.num = 204, .addr = 0xffffffff810ae930, .name = "sys_sched_getaffinity", .pre = 0x00, .classify = 4},
    {.num = 205, .addr = 0xffffffff8103d860, .name = "sys_set_thread_area", .pre = 0x00, .classify = 4},
    {.num = 206, .addr = 0xffffffff8125ec20, .name = "sys_io_setup", .pre = 0x00, .classify = 4},
    {.num = 207, .addr = 0xffffffff8125f640, .name = "sys_io_destroy", .pre = 0x00, .classify = 4},
    {.num = 208, .addr = 0xffffffff8125fd80, .name = "sys_io_getevents", .pre = 0x00, .classify = 4},
    {.num = 209, .addr = 0xffffffff8125fc40, .name = "sys_io_submit", .pre = 0x00, .classify = 4},
    {.num = 210, .addr = 0xffffffff8125fc60, .name = "sys_io_cancel", .pre = 0x00, .classify = 4},
    {.num = 211, .addr = 0xffffffff8103d920, .name = "sys_get_thread_area", .pre = 0x00, .classify = 4},
    {.num = 212, .addr = 0xffffffff81290ec0, .name = "sys_lookup_dcookie", .pre = 0x00, .classify = 3},
    {.num = 213, .addr = 0xffffffff81257d90, .name = "sys_epoll_create", .pre = 0x00, .classify = 3},
    {.num = 214, .addr = 0x0000000000000000, .name = "sys_epoll_ctl_old", .pre = 0x00, .classify = 3},
    {.num = 215, .addr = 0x0000000000000000, .name = "sys_epoll_wait_old", .pre = 0x00, .classify = 3},
    {.num = 216, .addr = 0xffffffff811c94d0, .name = "sys_remap_file_pages", .pre = 0x00, .classify = 3},
    {.num = 217, .addr = 0xffffffff81222a80, .name = "sys_getdents64", .pre = 0x00, .classify = 3},
    {.num = 218, .addr = 0xffffffff81080410, .name = "sys_set_tid_address", .pre = 0x00, .classify = 1},
    {.num = 219, .addr = 0xffffffff81090ed0, .name = "sys_restart_syscall", .pre = 0x00, .classify = 4},
    {.num = 220, .addr = 0xffffffff81333090, .name = "sys_semtimedop", .pre = 0x00, .classify = 1},
    {.num = 221, .addr = 0xffffffff811d2e50, .name = "sys_fadvise64", .pre = 0x00, .classify = 3},
    {.num = 222, .addr = 0xffffffff810f1dd0, .name = "sys_timer_create", .pre = 0x00, .classify = 4},
    {.num = 223, .addr = 0xffffffff810f2410, .name = "sys_timer_settime", .pre = 0x00, .classify = 4},
    {.num = 224, .addr = 0xffffffff810f2260, .name = "sys_timer_gettime", .pre = 0x00, .classify = 4},
    {.num = 225, .addr = 0xffffffff810f23a0, .name = "sys_timer_getoverrun", .pre = 0x00, .classify = 4},
    {.num = 226, .addr = 0xffffffff810f2600, .name = "sys_timer_delete", .pre = 0x00, .classify = 4},
    {.num = 227, .addr = 0xffffffff810f28d0, .name = "sys_clock_settime", .pre = 0x00, .classify = 4},
    {.num = 228, .addr = 0x0000000000000000, .name = "sys_clock_gettime", .pre = 0x00, .classify = 4},//0xffffffff810f2990
    {.num = 229, .addr = 0xffffffff810f2a50, .name = "sys_clock_getres", .pre = 0x00, .classify = 4},
    {.num = 230, .addr = 0xffffffff810f2b10, .name = "sys_clock_nanosleep", .pre = 0x00, .classify = 1},
    {.num = 231, .addr = 0xffffffff810848a0, .name = "sys_exit_group", .pre = 0x00, .classify = 1},
    {.num = 232, .addr = 0xffffffff81258bb0, .name = "sys_epoll_wait", .pre = 0x00, .classify = 3},
    {.num = 233, .addr = 0xffffffff81257e60, .name = "sys_epoll_ctl", .pre = 0x00, .classify = 3},
    {.num = 234, .addr = 0xffffffff81091a60, .name = "sys_tgkill", .pre = 0x00, .classify = 9},
    {.num = 235, .addr = 0xffffffff81243460, .name = "sys_utimes", .pre = 0x00, .classify = 3},
    {.num = 236, .addr = 0x0000000000000000, .name = "sys_vserver", .pre = 0x00, .classify = 0},
    {.num = 237, .addr = 0xffffffff811e4830, .name = "sys_mbind", .pre = 0x00, .classify = 5},
    {.num = 238, .addr = 0xffffffff811e3350, .name = "sys_set_mempolicy", .pre = 0x00, .classify = 5},
    {.num = 239, .addr = 0xffffffff811e35b0, .name = "sys_get_mempolicy", .pre = 0x00, .classify = 5},
    {.num = 240, .addr = 0xffffffff813368c0, .name = "sys_mq_open", .pre = 0x00, .classify = 9},
    {.num = 241, .addr = 0xffffffff81336c30, .name = "sys_mq_unlink", .pre = 0x00, .classify = 9},
    {.num = 242, .addr = 0xffffffff81336db0, .name = "sys_mq_timedsend", .pre = 0x00, .classify = 9},
    {.num = 243, .addr = 0xffffffff81337140, .name = "sys_mq_timedreceive", .pre = 0x00, .classify = 9},
    {.num = 244, .addr = 0xffffffff81337680, .name = "sys_mq_notify", .pre = 0x00, .classify = 9},
    {.num = 245, .addr = 0xffffffff81337af0, .name = "sys_mq_getsetattr", .pre = 0x00, .classify = 9},
    {.num = 246, .addr = 0xffffffff8110ea90, .name = "sys_kexec_load", .pre = 0x00, .classify = 5},
    {.num = 247, .addr = 0xffffffff810848f0, .name = "sys_waitid", .pre = 0x00, .classify = 1},
    {.num = 248, .addr = 0xffffffff8133b570, .name = "sys_add_key", .pre = 0x00, .classify = 4},
    {.num = 249, .addr = 0xffffffff8133b770, .name = "sys_request_key", .pre = 0x00, .classify = 4},
    {.num = 250, .addr = 0xffffffff8133cde0, .name = "sys_keyctl", .pre = 0x00, .classify = 4},
    {.num = 251, .addr = 0xffffffff813db790, .name = "sys_ioprio_set", .pre = 0x00, .classify = 1},
    {.num = 252, .addr = 0xffffffff813dbaa0, .name = "sys_ioprio_get", .pre = 0x00, .classify = 1},
    {.num = 253, .addr = 0xffffffff81254690, .name = "sys_inotify_init", .pre = 0x00, .classify = 2},
    {.num = 254, .addr = 0xffffffff812546f0, .name = "sys_inotify_add_watch", .pre = 0x00, .classify = 2},
    {.num = 255, .addr = 0xffffffff81254a40, .name = "sys_inotify_rm_watch", .pre = 0x00, .classify = 2},
    {.num = 256, .addr = 0xffffffff811e33e0, .name = "sys_migrate_pages", .pre = 0x00, .classify = 5},
    {.num = 257, .addr = 0xffffffff8120d9e0, .name = "sys_openat", .pre = 0x00, .classify = 2},
    {.num = 258, .addr = 0xffffffff8121f5d0, .name = "sys_mkdirat", .pre = 0x00, .classify = 3},
    {.num = 259, .addr = 0xffffffff8121f1d0, .name = "sys_mknodat", .pre = 0x00, .classify = 3},
    {.num = 260, .addr = 0xffffffff8120d0c0, .name = "sys_fchownat", .pre = 0x00, .classify = 3},
    {.num = 261, .addr = 0xffffffff81243380, .name = "sys_futimesat", .pre = 0x00, .classify = 3},
    {.num = 262, .addr = 0xffffffff81213d90, .name = "sys_newfstatat", .pre = 0x00, .classify = 3},
    {.num = 263, .addr = 0xffffffff8121f7f0, .name = "sys_unlinkat", .pre = 0x00, .classify = 3},
    {.num = 264, .addr = 0xffffffff812204c0, .name = "sys_renameat", .pre = 0x00, .classify = 3},
    {.num = 265, .addr = 0xffffffff8121fa50, .name = "sys_linkat", .pre = 0x00, .classify = 3},
    {.num = 266, .addr = 0xffffffff8121f840, .name = "sys_symlinkat", .pre = 0x00, .classify = 3},
    {.num = 267, .addr = 0xffffffff81213db0, .name = "sys_readlinkat", .pre = 0x00, .classify = 3},
    {.num = 268, .addr = 0xffffffff8120cf60, .name = "sys_fchmodat", .pre = 0x00, .classify = 3},
    {.num = 269, .addr = 0xffffffff8120c7f0, .name = "sys_faccessat", .pre = 0x00, .classify = 3},
    {.num = 270, .addr = 0xffffffff81223d80, .name = "sys_pselect6", .pre = 0x00, .classify = 7},
    {.num = 271, .addr = 0xffffffff812246c0, .name = "sys_ppoll", .pre = 0x00, .classify = 2},
    {.num = 272, .addr = 0xffffffff81080930, .name = "sys_unshare", .pre = 0x00, .classify = 1},
    {.num = 273, .addr = 0xffffffff81103360, .name = "sys_set_robust_list", .pre = 0x00, .classify = 9},
    {.num = 274, .addr = 0xffffffff811033a0, .name = "sys_get_robust_list", .pre = 0x00, .classify = 9},
    {.num = 275, .addr = 0xffffffff81241c20, .name = "sys_splice", .pre = 0x00, .classify = 2},
    {.num = 276, .addr = 0xffffffff812423b0, .name = "sys_tee", .pre = 0x00, .classify = 2},
    {.num = 277, .addr = 0xffffffff81242c80, .name = "sys_sync_file_range", .pre = 0x00, .classify = 2},
    {.num = 278, .addr = 0xffffffff812419d0, .name = "sys_vmsplice", .pre = 0x00, .classify = 2},
    {.num = 279, .addr = 0xffffffff811f4800, .name = "sys_move_pages", .pre = 0x00, .classify = 1},
    {.num = 280, .addr = 0xffffffff812432e0, .name = "sys_utimensat", .pre = 0x00, .classify = 3},
    {.num = 281, .addr = 0xffffffff81258c80, .name = "sys_epoll_pwait", .pre = 0x00, .classify = 3},
    {.num = 282, .addr = 0xffffffff81259bb0, .name = "sys_signalfd", .pre = 0x00, .classify = 9},
    {.num = 283, .addr = 0xffffffff8125aee0, .name = "sys_timerfd_create", .pre = 0x00, .classify = 4},
    {.num = 284, .addr = 0xffffffff8125bca0, .name = "sys_eventfd", .pre = 0x00, .classify = 9},
    {.num = 285, .addr = 0xffffffff8120c780, .name = "sys_fallocate", .pre = 0x00, .classify = 3},
    {.num = 286, .addr = 0xffffffff8125b000, .name = "sys_timerfd_settime", .pre = 0x00, .classify = 4},
    {.num = 287, .addr = 0xffffffff8125b0b0, .name = "sys_timerfd_gettime", .pre = 0x00, .classify = 4},
    {.num = 288, .addr = 0xffffffff8170fb80, .name = "sys_accept4", .pre = 0x00, .classify = 7},
    {.num = 289, .addr = 0xffffffff812599e0, .name = "sys_signalfd4", .pre = 0x00, .classify = 9},
    {.num = 290, .addr = 0xffffffff8125bc10, .name = "sys_eventfd2", .pre = 0x00, .classify = 9},
    {.num = 291, .addr = 0xffffffff81257cb0, .name = "sys_epoll_create1", .pre = 0x00, .classify = 3},
    {.num = 292, .addr = 0xffffffff8122cf40, .name = "sys_dup3", .pre = 0x00, .classify = 2},
    {.num = 293, .addr = 0xffffffff81217f50, .name = "sys_pipe2", .pre = 0x00, .classify = 9},
    {.num = 294, .addr = 0xffffffff81254620, .name = "sys_inotify_init1", .pre = 0x00, .classify = 2},
    {.num = 295, .addr = 0xffffffff8120fac0, .name = "sys_preadv", .pre = 0x00, .classify = 2},
    {.num = 296, .addr = 0xffffffff8120fba0, .name = "sys_pwritev", .pre = 0x00, .classify = 2},
    {.num = 297, .addr = 0xffffffff81091ae0, .name = "sys_rt_tgsigqueueinfo", .pre = 0x00, .classify = 9},
    {.num = 298, .addr = 0xffffffff81185f90, .name = "sys_perf_event_open", .pre = 0x00, .classify = 3},
    {.num = 299, .addr = 0xffffffff81710440, .name = "sys_recvmmsg", .pre = 0x00, .classify = 7},
    {.num = 300, .addr = 0xffffffff81256130, .name = "sys_fanotify_init", .pre = 0x00, .classify = 9},
    {.num = 301, .addr = 0xffffffff812563c0, .name = "sys_fanotify_mark", .pre = 0x00, .classify = 9},
    {.num = 302, .addr = 0xffffffff81095cc0, .name = "sys_prlimit64", .pre = 0x00, .classify = 4},
    {.num = 303, .addr = 0xffffffff81272470, .name = "sys_name_to_handle_at", .pre = 0x00, .classify = 3},
    {.num = 304, .addr = 0xffffffff81272920, .name = "sys_open_by_handle_at", .pre = 0x00, .classify = 3},
    {.num = 305, .addr = 0xffffffff810f2a40, .name = "sys_clock_adjtime", .pre = 0x00, .classify = 4},
    {.num = 306, .addr = 0xffffffff81242bd0, .name = "sys_syncfs", .pre = 0x00, .classify = 3},
    {.num = 307, .addr = 0xffffffff817100d0, .name = "sys_sendmmsg", .pre = 0x00, .classify = 7},
    {.num = 308, .addr = 0xffffffff810a19d0, .name = "sys_setns", .pre = 0x00, .classify = 0},
    {.num = 309, .addr = 0xffffffff810964b0, .name = "sys_getcpu", .pre = 0x00, .classify = 4},
    {.num = 310, .addr = 0xffffffff811d2b10, .name = "sys_process_vm_readv", .pre = 0x00, .classify = 1},
    {.num = 311, .addr = 0xffffffff811d2b30, .name = "sys_process_vm_writev", .pre = 0x00, .classify = 1},
    {.num = 312, .addr = 0xffffffff810ea1b0, .name = "sys_kcmp", .pre = 0x00, .classify = 1},
    {.num = 313, .addr = 0xffffffff8110b1b0, .name = "sys_finit_module", .pre = 0x00, .classify = 4},
    {.num = 314, .addr = 0xffffffff810ae190, .name = "sys_sched_setattr", .pre = 0x00, .classify = 1},
    {.num = 315, .addr = 0xffffffff810ae450, .name = "sys_sched_getattr", .pre = 0x00, .classify = 1},
    {.num = 316, .addr = 0xffffffff8121fef0, .name = "sys_renameat2", .pre = 0x00, .classify = 3},
    {.num = 317, .addr = 0xffffffff8113d4b0, .name = "sys_seccomp", .pre = 0x00, .classify = 4},
    {.num = 318, .addr = 0xffffffff81523bd0, .name = "sys_getrandom", .pre = 0x00, .classify = 4},
    {.num = 319, .addr = 0xffffffff811ad440, .name = "sys_memfd_create", .pre = 0x00, .classify = 5},
    {.num = 320, .addr = 0xffffffff8110fc90, .name = "sys_kexec_file_load", .pre = 0x00, .classify = 5},
    {.num = 321, .addr = 0xffffffff81174c20, .name = "sys_bpf", .pre = 0x00, .classify = 3},
    {.num = 322, .addr = 0xffffffff81216890, .name = "sys_execveat", .pre = 0x00, .classify = 1},
    {.num = 323, .addr = 0xffffffff8125d3d0, .name = "sys_userfaultfd", .pre = 0x00, .classify = 3},
    {.num = 324, .addr = 0xffffffff8118cd30, .name = "sys_membarrier", .pre = 0x00, .classify = 4},
    {.num = 325, .addr = 0xffffffff811c56e0, .name = "sys_mlock2", .pre = 0x00, .classify = 5}

};

reg_t rdi = 0;
reg_t rax = 0;
reg_t cr3 = 0;
uint8_t trap = 0xCC;
int trapnum = 0;
int singstepnum = 0;
int sys_num = -1;
int pipenum = -1;
int writen = -1;
event_response_t singlestep_cb(vmi_instance_t vmi, vmi_event_t *event)
{
    ///printf("enter one cb %d %x\n ",sys_num,trap);

    vmi_write_8_va(vmi, syscalls[sys_num].addr, 0, &trap);
    ++singstepnum;

   /// printf("%ld one step \n",rax);

    return VMI_EVENT_RESPONSE_TOGGLE_SINGLESTEP;
}

event_response_t trap_cb(vmi_instance_t vmi, vmi_event_t *event)
{


    vmi_get_vcpureg(vmi, &rax, RAX, event->vcpu_id);
    vmi_get_vcpureg(vmi, &cr3, CR3, event->vcpu_id);
    vmi_pid_t pid = vmi_dtb_to_pid(vmi, cr3);
    psyscall nowsyscall;
    nowsyscall.pid = pid;
    nowsyscall.sysnum = (unsigned int)rax;

    writen = write(pipenum, &nowsyscall, sizeof(psyscall));
    if(writen<1){
        printf("pipe write error!\n");
    }

    sys_num = rax;
    syscallnum[sys_num]++;

   /// printf("%d  syscall#=%u right:%x\n", pid,(unsigned int)rax,syscalls[sys_num].pre);
    vmi_write_8_va(vmi, syscalls[sys_num].addr, 0, &(syscalls[sys_num].pre));


    event->interrupt_event.reinject = 0;
    ///printf("return\n");

    ++trapnum;

    return VMI_EVENT_RESPONSE_TOGGLE_SINGLESTEP;
}

void combineSyscallAndPs(LinkQueue* queue,int sysnum[][11],int psnum){
    int i = 0;
    TaskNode* q = queue->front->next;
    while(q != NULL)
    {
        int j = 1;
        int totalsyscall = 0;
        for(; j<11; j++)
        {
            q->syscallnum[j-1] = sysnum[i][j];
            totalsyscall+=sysnum[i][j];
        }
        q->syscallnum[10] = totalsyscall;
        i++;
        q = q->next;
    }
}

