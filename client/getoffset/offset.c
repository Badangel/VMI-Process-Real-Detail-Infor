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

struct task_struct task;

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

	//task_struct show
	printk("-----------task_struct-----------");
	printk("total_size: 0x%x\n", task_size);
	printk("tasks_offset: 0x%x\n", ltasks);
	printk("state_offset: 0x%x\n", lstate);
	printk("flags_offset: 0x%x\n", lflags);
	printk("prio_offset: 0x%x\n", lprio);
	printk("static_prio_offset: 0x%x\n", lstatic_prio);
	printk("normal_prio_offset: 0x%x\n", lnormal_prio);
	printk("rt_priority_offset: 0x%x\n", lrt_priority);
	printk("pid_offset: 0x%x\n", lpid);
	printk("tgid_offset: 0x%x\n", ltgid);
	printk("common_offset: 0x%x\n", lcommon);
	printk("ioac_offset: 0x%x\n", lioac);
	printk("mm_offset: 0x%x\n", lmm);
	printk("fs_offset: 0x%x\n", lfs);
	printk("files_offset: 0x%x\n", lfiles);
	printk("delays_offset: 0x%x\n\n", ldelays);

	
}

void show_mm(void)
{
	struct mm_struct mm;
	unsigned long p_mm = (unsigned long)&mm;

	//mm_struct offset
	unsigned long mm_size = sizeof(mm);
	unsigned long lpgd = (unsigned long)(&(mm.pgd))- p_mm ;
	unsigned long laddr = (unsigned long)(&(mm.start_code)) - p_mm;
	
	//mm_struct  show
	printk("-----------mm_struct-----------");
	printk("total_size: 0x%x\n", mm_size);
	printk("pgd_offset: 0x%x\n", lpgd);
	printk("startcode_offset: 0x%x\n\n", laddr);
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
	printk("total_size: 0x%x\n", opera_size);
	printk("owner_offset: 0x%x\n", owner_addr);
	printk("read_offset: 0x%x\n", read_addr);
	printk("write_offset: 0x%x\n", write_addr);
	printk("open_offset: 0x%x\n\n", open_addr);
	//printk("aio_read_offset: 0x%x\n", aio_read_addr);
	//printk("readdir_offset: 0x%x\n\n", readdir_addr);
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
	printk("total_size: 0x%x\n", fs_size);
	printk("root_offset: 0x%x\n", lroot);
	printk("pwd_offset: 0x%x\n\n", lpwd);
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
	printk("total_size: 0x%x\n", files_size);
	printk("count_offset: 0x%x\n", lcount);
	printk("fdt_offset: 0x%x\n", lfdt);
	printk("fdtab_offset: 0x%x\n", lfdtab);
	printk("fd_array_offset: 0x%x\n\n", lfd_array);
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
	printk("total_size: 0x%x\n", fdt_size);
	printk("max_fds_offset: 0x%x\n", lmax_fds);
	printk("fd_offset: 0x%x\n", lfd);
	printk("fd_rcu_offset: 0x%x\n\n", lfd_rcu);
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

	//file  show
	printk("-----------file-----------");
	printk("total_size: 0x%x\n", file_size);
	printk("f_path_offset: 0x%x\n", lf_path);
	printk("f_inode_offset: 0x%x\n", lf_inode);
	printk("f_op_offset: 0x%x\n", lf_op);
	printk("f_count_offset: 0x%x\n\n", lf_count);
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
	printk("total_size: 0x%x\n", pa_size);
	printk("mnt_offset: 0x%x\n", lmnt);
	printk("dentry_offset: 0x%x\n\n", ldentry);
}

void show_dentry(void)
{
	struct dentry dir;
	struct qstr str;
	struct vfsmount vfs;
	struct inode ino;
	unsigned long p_fdir = (unsigned long)&dir;
	unsigned long p_qstr = (unsigned long)&str;
	unsigned long p_vfs = (unsigned long)&vfs;
	unsigned long p_ino = (unsigned long)&ino;

	//dentry offset
	unsigned long dir_size = sizeof(dir);
	unsigned long lparent = (unsigned long)(&(dir.d_parent)) - p_fdir;
	unsigned long lname = (unsigned long)(&(dir.d_name)) - p_fdir;
	
	//dentry  show
	printk("-----------dentry-----------");
	printk("total_size: 0x%x\n", dir_size);
	printk("parent_offset: 0x%x\n", lparent);
	printk("name_offset: 0x%x\n\n", lname);

	//qstr offset
	unsigned long qstr_size = sizeof(str);
	unsigned long lpname = (unsigned long)(&(str.name)) - p_qstr;
	
	//qstr  show
	printk("-----------qstr-----------");
	printk("total_size: 0x%x\n", qstr_size);
	printk("name_offset: 0x%x\n\n", lpname);

	//vfsmount offset
	unsigned long vfs_size = sizeof(vfs);
	unsigned long lmnt_root = (unsigned long)(&(vfs.mnt_root)) - p_vfs;
	unsigned long lmnt_sb = (unsigned long)(&(vfs.mnt_sb)) - p_vfs;

	//vfsmount  show
	printk("-----------vfsmount-----------");
	printk("total_size: 0x%x\n", vfs_size);
	printk("mnt_root_offset: 0x%x\n", lmnt_root);
	printk("mnt_sb_offset: 0x%x\n\n", lmnt_sb);

	//inode offset
	unsigned long ino_size = sizeof(ino);
	unsigned long liopflags = (unsigned long)(&(ino.i_opflags)) - p_ino;
	unsigned long liuid = (unsigned long)(&(ino.i_uid)) - p_ino;
	unsigned long ligid = (unsigned long)(&(ino.i_gid)) - p_ino;

	//inode  show
	printk("-----------inode-----------");
	printk("total_size: 0x%x\n", ino_size);
	printk("i_opflags_offset: 0x%x\n", liopflags);
	printk("i_uid_offset: 0x%x\n\n", liuid);
	printk("i_gid_offset: 0x%x\n\n", ligid);

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
	show_path();
	show_dentry();
	
	return 0;
}

static void hello_exit(void) {
	printk(KERN_WARNING "Module exit: bye-bye\n");
}

module_init(hello_init);
module_exit(hello_exit);

