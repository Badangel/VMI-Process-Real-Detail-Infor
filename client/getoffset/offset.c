#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/fs_struct.h>
#include <linux/fdtable.h>
#include <linux/file.h>
#include <linux/path.h>
#include <linux/mount.h>
#include <linux/net.h>
#include <net/sock.h>

struct task_struct task;
#define EMBEDDED_LEVELS 2
struct nameidata {
	struct path	path;
	struct qstr	last;
	struct path	root;
	struct inode	*inode; /* path.dentry.d_inode */
	unsigned int	flags;
	unsigned	seq, m_seq;
	int		last_type;
	unsigned	depth;
	int		total_link_count;
	struct saved {
		struct path link;
		void *cookie;
		const char *name;
		struct inode *inode;
		unsigned seq;
	} *stack, internal[EMBEDDED_LEVELS];
	struct filename	*name;
	struct nameidata *saved;
	unsigned	root_seq;
	int		dfd;
};

void show_task(void)
{
	unsigned long p = (unsigned long)&task;

	//task_struct offset
	unsigned long task_size = sizeof(task);
	unsigned long ltasks = (unsigned long)(&(task.tasks))- p ;
	unsigned long lstate = (unsigned long)(&(task.state))- p ;
	unsigned long lflags = (unsigned long)(&(task.flags))- p ;
	unsigned long lprio = (unsigned long)(&(task.prio))- p ;
	unsigned long lstatic_prio = (unsigned long)(&(task.static_prio))- p ;
	unsigned long lnormal_prio = (unsigned long)(&(task.normal_prio))- p ;
	unsigned long lrt_priority = (unsigned long)(&(task.rt_priority))- p ;
	unsigned long lpid = (unsigned long)(&(task.pid)) - p;
	unsigned long ltgid = (unsigned long)(&(task.tgid)) - p;
	unsigned long lcommon = (unsigned long)(&(task.comm)) - p;
	unsigned long lioac = (unsigned long)(&(task.ioac)) - p;
	unsigned long lmm = (unsigned long)(&(task.mm)) - p;
	unsigned long lfs = (unsigned long)(&(task.fs)) - p;
	unsigned long lfiles = (unsigned long)(&(task.files)) - p;
	unsigned long ldelays = (unsigned long)(&(task.delays)) - p;
	unsigned long lnameidata = (unsigned long)(&(task.nameidata)) - p;
	unsigned long lgroup_leader = (unsigned long)(&(task.group_leader)) - p;
	unsigned long lutime = (unsigned long)(&(task.utime)) - p;
	unsigned long lstime = (unsigned long)(&(task.stime)) - p;
	unsigned long lstart_time = (unsigned long)(&(task.start_time)) - p;
	unsigned long lreal_start_time = (unsigned long)(&(task.real_start_time)) - p;
	unsigned long lnsproxy = (unsigned long)(&(task.nsproxy)) - p;
	unsigned long lreal_parent = (unsigned long)(&(task.real_parent)) - p;
	unsigned long lparent = (unsigned long)(&(task.parent)) - p;
	unsigned long lbio_list = (unsigned long)(&(task.bio_list)) - p;
	unsigned long lsplice_pipe = (unsigned long)(&(task.splice_pipe)) - p;
	unsigned long lioac = (unsigned long)(&(task.ioac)) - p;

	//task_struct show
	printk("-----------task_struct-----------");
	printk("total_size: %d\n", task_size);
	printk("tasks_offset: %d\n", ltasks);
	printk("state_offset: %d\n", lstate);
	printk("flags_offset: %d\n", lflags);
	printk("prio_offset: %d\n", lprio);
	printk("static_prio_offset: %d\n", lstatic_prio);
	printk("normal_prio_offset: %d\n", lnormal_prio);
	printk("rt_priority_offset: %d\n", lrt_priority);
	printk("pid_offset: %d\n", lpid);
	printk("tgid_offset: %d\n", ltgid);
	printk("common_offset: %d\n", lcommon);
	printk("ioac_offset: %d\n", lioac);
	printk("mm_offset: %d\n", lmm);
	printk("fs_offset: %d\n", lfs);
	printk("files_offset: %d\n", lfiles);
	printk("nameidata_offset: %d\n", lnameidata);
	printk("group_leader_offset: %d\n", lgroup_leader);
	printk("utime_offset: %d\n", lutime);
	printk("utime_offset: %d\n", lstime);
	printk("start_time_offset: %d\n", lstart_time);
	printk("real_start_time_offset: %d\n", lreal_start_time);
	printk("nsproxy_time_offset: %d\n", lnsproxy);
	printk("task_real_parent_offset: %d\n", lreal_parent);
	printk("task_parent_offset: %d\n", lparent);
	printk("bio_list_offset: %d\n", lbio_list);
	printk("splice_pipe_offset: %d\n", lsplice_pipe);
	printk("ioac_offset: %d\n", lioac);
	printk("delays_offset: %d\n\n", ldelays);

	
}

void show_mm(void)
{
	struct mm_struct mm;
	unsigned long p_mm = (unsigned long)&mm;

	//mm_struct offset
	unsigned long mm_size = sizeof(mm);
	unsigned long lpgd = (unsigned long)(&(mm.pgd))- p_mm ;
	unsigned long mmusers = (unsigned long)(&(mm.mm_users)) - p_mm;
	unsigned long mmcount = (unsigned long)(&(mm.mm_count)) - p_mm;
	unsigned long scddr = (unsigned long)(&(mm.start_code)) - p_mm;
	unsigned long seddr = (unsigned long)(&(mm.end_code)) - p_mm;
	unsigned long topamap = (unsigned long)(&(mm.total_vm)) - p_mm;
	unsigned long lockpa = (unsigned long)(&(mm.locked_vm)) - p_mm;
	unsigned long stackpa = (unsigned long)(&(mm.stack_vm)) - p_mm;
	
	//mm_struct  show
	printk("-----------mm_struct-----------");
	printk("total_size: %d\n", mm_size);
	printk("mmusers_offset: %d\n", mmusers);
	printk("mmcount_offset: %d\n", mmcount);
	printk("start_code_offset: %d\n", scddr);
	printk("end_code_offset: %d\n", seddr);
	printk("stack_vm_offset: %d\n", stackpa);
	printk("total_vm_offset: %d\n", topamap);
	printk("locked_vm_offset: %d\n", lockpa);
	printk("pgd_offset: %d\n\n", lpgd);
	
}

void show_fopera(void)
{
	struct file_operations f_op;
	unsigned long p_fopera = (unsigned long)&f_op;

	//file_operations offset
	unsigned long opera_size = sizeof(f_op);
	unsigned long owner_addr = (unsigned long)(&(f_op.owner))- p_fopera ;
	unsigned long read_addr = (unsigned long)(&(f_op.read)) - p_fopera;
	unsigned long write_addr = (unsigned long)(&(f_op.write))- p_fopera ;
	unsigned long open_addr = (unsigned long)(&(f_op.open)) - p_fopera;
	//unsigned long aio_read_addr = (unsigned long)(&(f_op.aio_read)) - p_fopera;
	//unsigned long readdir_addr = (unsigned long)(&(f_op.readdir)) - p_fopera;

	//file_operations show
	printk("-----------file_operations-----------");
	printk("total_size: %d\n", opera_size);
	printk("owner_offset: %d\n", owner_addr);
	printk("read_offset: %d\n", read_addr);
	printk("write_offset: %d\n", write_addr);
	printk("open_offset: %d\n\n", open_addr);
	//printk("aio_read_offset: %d\n", aio_read_addr);
	//printk("readdir_offset: %d\n\n", readdir_addr);
}

void show_fs(void)
{

	struct fs_struct fs;
	unsigned long p_fs = (unsigned long)&fs;

	//fs_struct offset
	unsigned long fs_size = sizeof(fs);
	unsigned long lroot = (unsigned long)(&(fs.root))- p_fs;
	unsigned long lpwd = (unsigned long)(&(fs.pwd)) - p_fs;
	
	//fs_struct  show
	printk("-----------fs_struct-----------");
	printk("total_size: %d\n", fs_size);
	printk("root_offset: %d\n", lroot);
	printk("pwd_offset: %d\n\n", lpwd);
}

void show_files_struct(void)
{

	struct files_struct fls;
	unsigned long p_files = (unsigned long)&fls;

	//files_struct offset
	unsigned long files_size = sizeof(fls);
	
	unsigned long lcount = (unsigned long)(&(fls.count)) - p_files;
	unsigned long lfdt = (unsigned long)(&(fls.fdt)) - p_files;
	unsigned long lfdtab = (unsigned long)(&(fls.fdtab)) - p_files;
	unsigned long lfd_array = (unsigned long)(&(fls.fd_array)) - p_files;
	
	//files_struct  show
	printk("-----------files_struct-----------");
	printk("total_size: %d\n", files_size);
	printk("count_offset: %d\n", lcount);
	printk("fdt_offset: %d\n", lfdt);
	printk("fdtab_offset: %d\n", lfdtab);
	printk("fd_array_offset: %d\n\n", lfd_array);
}

void show_fdtable(void)
{

	struct fdtable fdt1;
	unsigned long p_fdt1 = (unsigned long)&fdt1;

	//show fdtable offset
	unsigned long fdt_size = sizeof(fdt1);
	unsigned long lmax_fds = (unsigned long)(&(fdt1.max_fds))- p_fdt1;
	unsigned long lfd = (unsigned long)(&(fdt1.fd)) - p_fdt1;
	unsigned long lfd_rcu = (unsigned long)(&(fdt1.rcu)) - p_fdt1;
	
	//files_struct  show
	printk("-----------fdtable_struct-----------");
	printk("total_size: %d\n", fdt_size);
	printk("max_fds_offset: %d\n", lmax_fds);
	printk("fd_offset: %d\n", lfd);
	printk("fd_rcu_offset: %d\n\n", lfd_rcu);
}
void show_file(void)
{
	struct file f;
	unsigned long p_f = (unsigned long)&f;

	//show file offset
	unsigned long file_size = sizeof(f);
	unsigned long lf_path = (unsigned long)(&(f.f_path))- p_f;
	unsigned long lf_inode = (unsigned long)(&(f.f_inode)) - p_f;
	unsigned long lf_op = (unsigned long)(&(f.f_op)) - p_f;
	unsigned long lf_count = (unsigned long)(&(f.f_count)) - p_f;
	unsigned long lf_flags = (unsigned long)(&(f.f_flags)) - p_f;
	unsigned long lprivate_data = (unsigned long)(&(f.private_data)) - p_f;
        unsigned long lf_mode = (unsigned long)(&(f.f_mode)) - p_f;
	unsigned long lf_pos_lock = (unsigned long)(&(f.f_pos_lock)) - p_f;

	//file  show
	printk("-----------file-----------");
	printk("total_size: %d\n", file_size);
	printk("f_path_offset: %d\n", lf_path);
	printk("f_inode_offset: %d\n", lf_inode);
	printk("f_op_offset: %d\n", lf_op);
	printk("f_flags_offset: %d\n", lf_flags);
	printk("f_private_data_offset: %d\n", lprivate_data);
	printk("f_count_offset: %d\n", lf_count);
	printk("f_mode_offset: %d\n", lf_mode);
	printk("f_pos_lock_offset: %d\n\n", lf_pos_lock);
}

void show_socket(void)
{

	struct socket sock1;
	unsigned long p_sock = (unsigned long)&sock1;

	//socket offset
	unsigned long socket_size = sizeof(sock1);
	unsigned long lstate = (unsigned long)(&(sock1.state))- p_sock;
	unsigned long lsocket_flags = (unsigned long)(&(sock1.flags)) - p_sock;
	unsigned long lsocket_file = (unsigned long)(&(sock1.file)) - p_sock;
	unsigned long lsk = (unsigned long)(&(sock1.sk)) - p_sock;
	
	//socket  show
	printk("-----------socket_struct-----------");
	printk("total_size: %d\n", socket_size);
	printk("state_offset: %d\n", lstate);
	printk("socket_flags_offset: %d\n", lsocket_flags);
	printk("socket_file_offset: %d\n", lsocket_file);
	printk("socket_sk_offset: %d\n\n", lsk);
}

void show_sock(void)
{

	struct sock sk1;
	unsigned long p_sk = (unsigned long)&sk1;

	//sock offset
	unsigned long sock_size = sizeof(sk1);
	unsigned long l__sk_common = (unsigned long)(&(sk1.__sk_common))- p_sk;
	unsigned long l__sk_common_skc_daddr = (unsigned long)(&(sk1.__sk_common.skc_daddr)) - p_sk;
	unsigned long l__sk_common_skc_rcv_saddr = (unsigned long)(&(sk1.__sk_common.skc_rcv_saddr)) - p_sk;
	unsigned long lsk_daddr = (unsigned long)(&(sk1.sk_daddr)) - p_sk;
	unsigned long lsk_rcv_saddr = (unsigned long)(&(sk1.sk_rcv_saddr)) - p_sk;
	unsigned long lsk_num = (unsigned long)(&(sk1.sk_num)) - p_sk;
	unsigned long lsk_dport = (unsigned long)(&(sk1.sk_dport)) - p_sk;
	unsigned long lsk_prot = (unsigned long)(&(sk1.sk_prot)) - p_sk;
	unsigned long lsk_v6_daddr = (unsigned long)(&(sk1.sk_v6_daddr)) - p_sk;
	unsigned long lsk_v6_rcv_saddr = (unsigned long)(&(sk1.sk_v6_rcv_saddr)) - p_sk;

	
	//sock  show
	printk("-----------sock_struct-----------");
	printk("total_size: %d\n", sock_size);
	printk("__sk_common_offset: %d\n", l__sk_common);
	printk("__sk_common_skc_daddr_offset: %d\n", l__sk_common_skc_daddr);
	printk("__sk_common_skc_rcv_saddr_offset: %d\n", l__sk_common_skc_rcv_saddr);
	printk("sk_daddr_offset: %d\n", lsk_daddr);
	printk("sk_rcv_saddr_offset: %d\n", lsk_rcv_saddr);
	printk("sk_num_offset: %d\n", lsk_num);
	printk("sk_dport_offset: %d\n", lsk_dport);
	printk("sk_prot_offset: %d\n", lsk_prot);
	printk("sk_v6_daddr_offset: %d\n", lsk_v6_daddr);
	printk("sk_v6_rcv_saddr_offset: %d\n\n", lsk_v6_rcv_saddr);
}

void show_path(void)
{

	struct path pa;
	unsigned long p_pa = (unsigned long)&pa;

	//path offset
	unsigned long pa_size = sizeof(pa);
	unsigned long lmnt = (unsigned long)(&(pa.mnt))- p_pa;
	unsigned long ldentry = (unsigned long)(&(pa.dentry)) - p_pa;
	
	//path  show
	printk("-----------path_struct-----------");
	printk("total_size: %d\n", pa_size);
	printk("mnt_offset: %d\n", lmnt);
	printk("dentry_offset: %d\n\n", ldentry);
}

void show_dentry(void)
{
	struct dentry dir;
	struct qstr str;
	struct vfsmount vfs;
	struct inode ino;
	struct nameidata nameida;
	struct module mod;	
	unsigned long p_mod = (unsigned long)&mod;
	unsigned long p_fdir = (unsigned long)&dir;
	unsigned long p_qstr = (unsigned long)&str;
	unsigned long p_vfs = (unsigned long)&vfs;
	unsigned long p_ino = (unsigned long)&ino;
	unsigned long p_nameida = (unsigned long)&nameida;

	//dentry offset
	unsigned long dir_size = sizeof(dir);
	unsigned long lparent = (unsigned long)(&(dir.d_parent)) - p_fdir;
	unsigned long lname = (unsigned long)(&(dir.d_name)) - p_fdir;
	
	//dentry  show
	printk("-----------dentry-----------");
	printk("total_size: %d\n", dir_size);
	printk("parent_offset: %d\n", lparent);
	printk("name_offset: %d\n\n", lname);

	//qstr offset
	unsigned long qstr_size = sizeof(str);
	unsigned long lpname = (unsigned long)(&(str.name)) - p_qstr;
	
	//qstr  show
	printk("-----------qstr-----------");
	printk("total_size: %d\n", qstr_size);
	printk("name_offset: %d\n\n", lpname);

	//vfsmount offset
	unsigned long vfs_size = sizeof(vfs);
	unsigned long lmnt_root = (unsigned long)(&(vfs.mnt_root)) - p_vfs;
	unsigned long lmnt_sb = (unsigned long)(&(vfs.mnt_sb)) - p_vfs;

	//vfsmount  show
	printk("-----------vfsmount-----------");
	printk("total_size: %d\n", vfs_size);
	printk("mnt_root_offset: %d\n", lmnt_root);
	printk("mnt_sb_offset: %d\n\n", lmnt_sb);

	//inode offset
	unsigned long ino_size = sizeof(ino);
	unsigned long liopflags = (unsigned long)(&(ino.i_opflags)) - p_ino;
	unsigned long liuid = (unsigned long)(&(ino.i_uid)) - p_ino;
	unsigned long ligid = (unsigned long)(&(ino.i_gid)) - p_ino;

	//inode  show
	printk("-----------inode-----------");
	printk("total_size: %d\n", ino_size);
	printk("i_opflags_offset: %d\n", liopflags);
	printk("i_uid_offset: %d\n", liuid);
	printk("i_gid_offset: %d\n\n", ligid);
	
	//nameidata offset
	unsigned long nameidata_size = sizeof(nameida);
	unsigned long llast = (unsigned long)(&(nameida.last)) - p_nameida;
	unsigned long lnameidata_root = (unsigned long)(&(nameida.root)) - p_nameida;
	unsigned long lnameidata_inode = (unsigned long)(&(nameida.inode)) - p_nameida;
	unsigned long lnameidata_depth = (unsigned long)(&(nameida.depth)) - p_nameida;
	unsigned long lnameidate_name = (unsigned long)(&(nameida.name)) - p_nameida;

	//nameidata  show
	printk("-----------nameidata-----------");
	printk("total_size: %d\n", nameidata_size);
	printk("llast_offset: %d\n", llast);
	printk("nameidata_root_offset: %d\n", lnameidata_root);
	printk("nameidata_inode_offset: %d\n", lnameidata_inode);
	printk("nameidata_depth_offset: %d\n", lnameidata_depth);
	printk("nameidate_name_offset: %d\n\n", lnameidate_name);

	//module offset
	unsigned long mod_size = sizeof(mod);
	unsigned long lnum_syms = (unsigned long)(&(mod.num_syms)) - p_mod;
	unsigned long lnum_kp = (unsigned long)(&(mod.num_kp)) - p_mod;

	//module  show
	printk("-----------module-----------");
	printk("module_size: %d\n", mod_size);
	printk("num_syms_offset: %d\n", lnum_syms);
	printk("num_kp_offset: %d\n\n", lnum_kp);


}

static int hello_init(void) {
	printk(KERN_WARNING "Module init: Hello world!\n");
	
	//offset count
	show_task();
	show_mm();
	show_fopera();
	show_fs();
	show_files_struct();
	show_fdtable();
	show_file();
	show_socket();
	show_sock();
	show_path();
	show_dentry();
	
	return 0;
}

static void hello_exit(void) {
	printk(KERN_WARNING "Module exit: bye-bye\n");
}

module_init(hello_init);
module_exit(hello_exit);

