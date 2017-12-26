/**
* Copyright(c) 2017 dyf
* record task_struct/files/fd/sk/dentry... offset
* give some funtions to get infomation about one task deeply
*/

#ifndef MYPSINFO_H_INCLUDED
#define MYPSINFO_H_INCLUDED

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

/**
 * This funtion get mm_struct info
 */
void get_mm_struct(vmi_instance_t vmi, addr_t currentps, TaskNode *tmptn)
{
    addr_t currmm_structaddr = 1;
  
    uint32_t cmm_users;
    uint32_t cmm_count;
    uint64_t cpgd;
    unsigned long ctotal_vm;
    uint64_t clocked_cm;
    unsigned long cstack_vm;
    uint64_t cstart_code;
    uint64_t cend_code;
    vmi_read_addr_va(vmi,currentps+mm_offset,0,&currmm_structaddr);
    vmi_read_32_va(vmi,currmm_structaddr+mm_users_offset,0,&cmm_users);
    vmi_read_32_va(vmi,currmm_structaddr+mm_count_offset,0,&cmm_count);
    vmi_read_64_va(vmi,currmm_structaddr+pgd_offset,0,&cpgd);
    vmi_read_64_va(vmi,currmm_structaddr+total_vm_offset,0,&ctotal_vm);
    vmi_read_64_va(vmi,currmm_structaddr+locked_vm_offset,0,&clocked_cm);
    vmi_read_64_va(vmi,currmm_structaddr+stack_vm_offset,0,&cstack_vm);
    vmi_read_64_va(vmi,currmm_structaddr+start_code_offset,0,&cstart_code);
    vmi_read_64_va(vmi,currmm_structaddr+end_code_offset,0,&cend_code);
    tmptn->tsmm_users = cmm_users;
    tmptn->tsmm_count = cmm_count;
    if(cmm_count==0){
        tmptn->tsmm_users = 0;
    }
    tmptn->tstotal_vm = ctotal_vm;
    tmptn->tsstack_vm = cstack_vm;
    /**stack_vm to large may no data or system space*/
    if(cstack_vm > 5000){
     tmptn->tsstack_vm = 0;   
    }
    printf("mm_users:%d mm_count:%d pgd:%p total_vm:%ld locked_cm:%ld stack_vm:%ld start_code:%p end_code:%p\n",cmm_users,cmm_count,(void*)cpgd,ctotal_vm,clocked_cm,cstack_vm,(void*)cstart_code,(void*)cend_code);
}

/**
*This funtion get the dentry qstr name.
*/
char* get_dentry_name(vmi_instance_t vmi,char *name,addr_t dentry_addr)
{
    char qstr_d_name[255] = "";
    //qstr_d_name = (char*)malloc(255);
    char c = 1;
    vmi_read_addr_va(vmi, dentry_addr + dname_offset + qname_offset, 0, &p_name);
    //while(c != NULL && filetype != 0)
    while(c != '\0')
    {
        vmi_read_8_va(vmi, p_name, 0, &c);
        p_name = p_name + nadd;
        // printf("%c",c);
        char c_rmp[2]= {c};
        strcat(qstr_d_name,c_rmp);

    }
    // printf("qstr:%s ",qstr_d_name);
    strcpy(name,qstr_d_name);
    return name;

}///end get_dentry_name

/**
*This funtion for show the relevant files infomations.
*Include files socket and so on.
*/
void get_files_info(vmi_instance_t vmi,addr_t fdaddr,unsigned int max_file, TaskNode *tmp)
{
    int i;
    tmp->tsfdnum = 0;


    for(i=0; i<max_file; i++)
    {

        vmi_read_addr_va(vmi, fdaddr, 0, &file);
        fdaddr = fdaddr + fdadd;
        if(file!=0)
        {
            tmp->tsfdnum = tmp->tsfdnum + 1;
            ///printf("%3d  ",i);
            int filetype = 50;
            //vmi_read_addr_va(vmi, file + 16, 0, &f_path);
            vmi_read_addr_va(vmi, file + path_offset+dentry_offset, 0, &dentry1);
            vmi_read_addr_va(vmi, file + path_offset, 0, &mnt);
            vmi_read_addr_va(vmi, file + i_node_offset, 0, &i_node);

            /* read flags*/
            vmi_read_32_va(vmi, file + f_flags_offset, 0, &flags);
            ///printf("flag:%u ",flags);

            /* read the fmode */
            vmi_read_32_va(vmi, file + f_mode_offset, 0, &fmode);
            int j = 0;
            ///printf("fmode:%u ",fmode);

            vmi_read_addr_va(vmi, dentry1 + dname_offset, 0, &d_name);
            //unsigned char *xname = vmi_read_str_va(vmi, dentry1 + 40, 0);
            vmi_read_addr_va(vmi, dentry1 + dname_offset + qname_offset, 0, &f_name);

            uint32_t imode;
            unsigned int kuid,kgid;
            vmi_read_32_va(vmi, i_node, 0, &imode);
            vmi_read_32_va(vmi, i_node + i_uid_offset, 0, &kuid);
            vmi_read_32_va(vmi, i_node + i_gid_offset, 0, &kgid);
            ///printf("flag:%u fmode:%u imode:%u ku:gid([%4d]:[%4d]) ",flags,fmode,imode,kuid,kgid);


            vmi_read_addr_va(vmi, mnt, 0, &droot);
            vmi_read_addr_va(vmi, droot + dname_offset + qname_offset, 0, &rname);


            char vfsname[255]="";
            get_dentry_name(vmi,vfsname,droot);

            //strcat(vfsname,get_dentry_name(vmi,droot));
            if(vfsname[0]!='/')
            {
                filetype=0;
                ///printf("vn:%s ",vfsname);
            }
            else
            {
                tmp->fileinfo = tmp->fileinfo + 1;
            }

            char a = '1';
            int tcp = 1;
            char filename[255]="";
            //strcat(filename,get_dentry_name(vmi,dentry1));
            get_dentry_name(vmi,filename,dentry1);
            ///printf("fs:%s",filename);

            if(strcmp(vfsname,"pipe:")==0)tmp->pipeinfo = tmp->pipeinfo+1;
            if(strcmp(vfsname,"\0")==0)tmp->nullinfo = tmp->nullinfo+1;
            if(strcmp(filename,"TCPv6")==0)
            {
                tcp = 0;
                tmp->socketinfo[3]=tmp->socketinfo[3]+1;
            }

            if(vfsname[0] != '/'&&tcp == 1)
            {
                if(strcmp(filename,"UNIX")==0)tmp->socketinfo[0] = tmp->socketinfo[0] + 1;
                if(strcmp(filename,"NETLINK")==0)tmp->socketinfo[1] = tmp->socketinfo[1] + 1;
                if(strcmp(filename,"TCP")==0)tmp->socketinfo[2] = tmp->socketinfo[2] + 1;
                if(strcmp(filename,"UDP")==0)tmp->socketinfo[3] = tmp->socketinfo[3] + 1;
                if(strcmp(filename,"[eventfd]")==0)tmp->anon_inodeinfo[0] = tmp->anon_inodeinfo[0]+1;
                if(strcmp(filename,"inotify")==0)tmp->anon_inodeinfo[1] = tmp->anon_inodeinfo[1]+1;
                if(strcmp(filename,"[timerfd]")==0)tmp->anon_inodeinfo[2] = tmp->anon_inodeinfo[2]+1;
                if(strcmp(filename,"[signalfd]")==0)tmp->anon_inodeinfo[3] = tmp->anon_inodeinfo[3]+1;
                if(strcmp(filename,"[eventpoll]")==0)tmp->anon_inodeinfo[4] = tmp->anon_inodeinfo[4]+1;
                vmi_read_addr_va(vmi, file + private_date_offset, 0, &private_date);
                vmi_read_addr_va(vmi, private_date + socket_sk_offset, 0, &sk);
                vmi_read_32_va(vmi, sk + sk_daddr_offset, 0, &daddr);
                vmi_read_32_va(vmi, sk + sk_rcv_saddr_offset, 0, &rcv_saddr);
                unsigned char* rcvIp = (unsigned char*)&rcv_saddr;
                unsigned char* dIp = (unsigned char*)&daddr;
                vmi_read_16_va(vmi, sk + sk_num_offset, 0, &sk_num);
                vmi_read_16_va(vmi, sk + sk_dport_offset, 0, &sk_dport);
                // printf("local:%x(num:%x)  source:%x(port:%x)",rcv_saddr,sk_num,daddr,sk_dport);
                ///printf(" %d.%d.%d.%d:%d <--- ",rcvIp[0],rcvIp[1],rcvIp[2],rcvIp[3],sk_num);
                ///printf("%d.%d.%d.%d:%d ",dIp[0],dIp[1],dIp[2],dIp[3],sk_dport);

            }
            if(tcp == 0)
            {

                vmi_read_addr_va(vmi, file + private_date_offset, 0, &private_date);
                vmi_read_addr_va(vmi, private_date + socket_sk_offset, 0, &sk);
                vmi_read_64_va(vmi, sk + sk_v6_daddr_offet, 0, &v6_daddr);
                vmi_read_64_va(vmi, sk + sk_v6_rcv_saddr_offet, 0, &v6_rcv_saddr);
                unsigned char* v6dIp = (unsigned char*)&v6_daddr;
                unsigned char* v6rcvIp = (unsigned char*)&v6_rcv_saddr;

                vmi_read_16_va(vmi, sk + sk_num_offset, 0, &sk_num);
                vmi_read_16_va(vmi, sk + sk_dport_offset, 0, &sk_dport);
                // printf(" %x: <-- ",v6_rcv_saddr);
                ///printf(" %x%02x:%x%02x:%x%02x:%x%02x :%d <-- ",v6rcvIp[0],v6rcvIp[1],v6rcvIp[2],v6rcvIp[3],v6rcvIp[4],v6rcvIp[5],v6rcvIp[6],v6rcvIp[7],sk_num);
               /// printf("%x%02x:%x%02x:%x%02x:%x%02x :%d ",v6dIp[0],v6dIp[1],v6dIp[2],v6dIp[3],v6dIp[4],v6dIp[5],v6dIp[6],v6dIp[7],sk_dport);
            }
            ///printf("|| ");
            vmi_read_addr_va(vmi, dentry1 + parent_offset, 0, &d_parent);
            //printf(" %x end",d_parent);
            a = 1;
            char pa='1';
            int n = 0;

            while(d_parent != 0 && n < filetype)
            {
                n=n+1;
                // vmi_read_addr_va(vmi, d_parent + dname_offset + qname_offset, 0, &p_name);
                // a=1;
                char parentname[255]="";
                get_dentry_name(vmi,parentname,d_parent);
                ///printf("%s",parentname);
                if(strcmp(parentname,"/")==0)
                {
                    filetype = 0;
                }

                ///printf("  ");
                vmi_read_addr_va(vmi, d_parent + parent_offset, 0, &d_parent);
            }
            ///printf("||%d \n",n);

            ////////////////////////////////////////////////////
            /*small with dentry qstr name.
            addr_t di_name = dentry1 + di_name_offset;
            a = '1';
            while(a!=NULL){
                vmi_read_8_va(vmi,di_name,0,&a);
                di_name = di_name + nadd;
                printf("%c",a);
            }
            printf(" \n----------\n");
            */
            ///////////////////////////////////////////////////////////////

        }
    }
    //fdinfoaddr = &tmp;
}///end get_files_info

/**
*This funtion get the qstr name.
*/
char* get_qstr_name(vmi_instance_t vmi,char *name,addr_t qstr_addr)
{
    char qstr_name[255] = "";
    //qstr_name = (char*)malloc(255);
    //qstr_d_name[0] = '';
    char c = 1;
    //while(c != NULL && filetype != 0)
    printf("??");
    while(c != '\0')
    {
        vmi_read_8_va(vmi, qstr_addr, 0, &c);
        qstr_addr = qstr_addr + nadd;
        //printf("%c",c);
        char c_rmp[2]= {c};
        strcat(qstr_name,c_rmp);

    }
    strcpy(name,qstr_name);
    // printf("qstr:%s ",qstr_d_name);
    return name;
}///end get_qstr_name



/**
*4.4.57 nameidata is null...
*/
void get_nameidata_info(vmi_instance_t vmi, addr_t nameidat)
{
    addr_t namei_path = nameidat + nameidata_path_offset;
    addr_t namei_last = nameidat + nameidata_last_offset;
    addr_t namei_root = nameidat + nameidata_root_offset;


    //char *getname;
    char nameipath[255]="";
    // get_dentry_name(vmi,namei_path+dentry_offset);
    get_dentry_name(vmi,nameipath,namei_path+dentry_offset);
    printf("nameipath:%s  ",nameipath);


    char nameilast[255]="";
    //get_qstr_name(vmi,namei_last+8);
    get_qstr_name(vmi,nameilast,namei_last+qname_offset);

    printf("nameilast:%s  ",nameilast);


    char nameiroot[255]="";
    get_dentry_name(vmi,nameiroot,namei_root+dentry_offset);
    printf("nameipath:%s\n",nameiroot);


}///end get_nameidata_info

/**
* show fs information
* only pwd is not null, fs other is null
*/
void get_fs_info(vmi_instance_t vmi, addr_t fs)
{
    vmi_read_addr_va(vmi, fs + fsroot_offset + dentry_offset, 0, &fsroot_dentry);
    vmi_read_addr_va(vmi, fs + fspwd_offset + dentry_offset, 0, &fspwd_dentry);
    if(fsroot_dentry != 0&& fspwd_dentry != 0)//!!!!!befor != NULL
    {
        addr_t vfs_mnt = 0;
        addr_t mnt_dentry = 0;
        vmi_read_addr_va(vmi, fs + fsroot_offset, 0, &vfs_mnt);
        vmi_read_addr_va(vmi, vfs_mnt, 0, &mnt_dentry);
        char rootvfsdentry[255]="";
        get_dentry_name(vmi,rootvfsdentry,mnt_dentry);
        ///printf("rootvfs:%s ",rootvfsdentry);

        char rootdentry[255]="";
        get_dentry_name(vmi,rootdentry,fsroot_dentry);
        ///printf("root:%s ",rootdentry);


        vmi_read_addr_va(vmi, fs + fspwd_offset, 0, &vfs_mnt);
        vmi_read_addr_va(vmi, vfs_mnt, 0, &mnt_dentry);
        char pwdvfsdentry[255]="";
        get_dentry_name(vmi,pwdvfsdentry,mnt_dentry);
        ///printf("pwdvfs:%s ",pwdvfsdentry);


        char pwddentrystr[255]="";
        get_dentry_name(vmi,pwddentrystr,fspwd_dentry);
        int n = 3;
        while(n > 0 && pwddentrystr[0] != '/')
        {
            //n = n - 1;
            char pwddentry[255]="";
            vmi_read_addr_va(vmi, fspwd_dentry + parent_offset, 0, &fspwd_dentry);
            strcat(pwddentrystr,"/");
            get_dentry_name(vmi,pwddentry,fspwd_dentry);
            if(pwddentry[0] == '/')
            {
                n = 0;
            }
            else
            {
                strcat(pwddentrystr,pwddentry);
            }
        }
        ///printf("pwd:%s \n",pwddentrystr);
    }
}///end get_fs_info

void get_task_info(vmi_instance_t vmi,addr_t current_process, TaskNode *tmptn)
{
    vmi_read_32_va(vmi, current_process + pid_offset, 0, (uint32_t*)&pid);
    vmi_read_32_va(vmi, current_process + tgid_offset, 0, (uint32_t*)&tgid);

    vmi_read_addr_va(vmi, current_process + task_real_parent_offset, 0, &task_real_parent);
    vmi_read_32_va(vmi, task_real_parent + pid_offset, 0, (uint32_t*)&real_parent_pid);
    vmi_read_addr_va(vmi, current_process + task_parent_offset, 0, &task_parent);
    vmi_read_32_va(vmi, task_parent + pid_offset, 0, (uint32_t*)&parent_pid);

    tmptn->tspid = pid;
    tmptn->tsparent = real_parent_pid;

    procname = vmi_read_str_va(vmi, current_process + name_offset, 0);
    strcpy(tmptn->tsname,procname);

    /*
    if (!procname)
    {
        printf("Failed to find procname\n");
        goto error_exit;
    }
    */

    /* print out the process name */
    printf("\n[%5d] [%5d] %s (struct addr:%"PRIx64")\n   rparent:%d  parent:%d ", pid, tgid, procname, current_process, real_parent_pid, parent_pid);
    ///printf("%d",pid);

    /*only show mm_struct info not add in struct*/
    get_mm_struct(vmi, current_process,tmptn);

    vmi_read_addr_va(vmi, current_process + group_leader_offset, 0, &group_leader);
    vmi_read_32_va(vmi, group_leader + pid_offset, 0, (uint32_t*)&group_leader_pid);
    tmptn->tsgroupleader = group_leader_pid;
    ///printf("groupleader:%d ",group_leader_pid);

    /* show fs information */
    ///printf("fb");
    vmi_read_addr_va(vmi, current_process + fs_offset, 0, &fs);
    get_fs_info(vmi, fs);
    ///printf("fa");

    /* show start time */
    uint64_t starttime = 0;
    vmi_read_64_va(vmi, current_process + start_time_offset, 0, (uint64_t*)&starttime);
    tmptn->tsstart_time = starttime;
    uint64_t realstarttime = 0;
    vmi_read_64_va(vmi, current_process + real_start_time_offset, 0, &realstarttime);
    tmptn->tsstart_time = starttime;
    tmptn->tsrealstart_time = realstarttime;
    ///printf("starttime:%lu  realstarttime:%lu ",starttime,realstarttime);

    /* show mm fault and swap info */
    unsigned long minflt = 0;
    vmi_read_64_va(vmi, current_process + min_flt_offset, 0, &minflt);
    unsigned long majflt = 0;
    vmi_read_64_va(vmi, current_process + maj_flt_offset, 0, &majflt);
    tmptn->tsminflt = minflt;
    tmptn->tsinc_minflt = minflt;
    tmptn->tsmajflt = majflt;
    tmptn->tsinc_majflt = majflt;
    ///printf("minflt:%lu majflt:%lu \n", minflt, majflt);

    /* show task prio */
    // first num same,the last num equal 0
    int prio, static_prio, normal_prio;
    unsigned int rt_priority;
    vmi_read_32_va(vmi, current_process + prio_offset, 0, &prio);
    vmi_read_32_va(vmi, current_process + static_prio_offset, 0, &static_prio);
    vmi_read_32_va(vmi, current_process + normal_prio_offset, 0, &normal_prio);
    vmi_read_32_va(vmi, current_process + rt_priority_offset, 0, &rt_priority);
    tmptn->tsprio = prio;
    printf("prio:%d static_prio:%d normal_prio:%d rt_priority:%u \n", prio, static_prio, normal_prio, rt_priority);


    /* show utime,stime,gtime */
    unsigned long utime = 0;
    unsigned long stime = 0;
    unsigned long utimescaled = 0;
    unsigned long stimescaled = 0;
    unsigned long gtime = 0;
    vmi_read_64_va(vmi, current_process + utime_offset, 0, &utime);
    vmi_read_64_va(vmi, current_process + stime_offset, 0, &stime);
    vmi_read_64_va(vmi, current_process + stime_offset+8, 0, &utimescaled);
    vmi_read_64_va(vmi, current_process + stime_offset+16, 0, &stimescaled);
    vmi_read_64_va(vmi, current_process + gtime_offset, 0, &gtime);
    tmptn->tsutime = utime;
    tmptn->tsstime = stime;
    tmptn->tsinc_utime = utime;
    tmptn->tsinc_stime = stime;
    printf("utime:%lu stime:%lu utimescaled:%lu stimescaled:%lu gtime:%lu\n", utime, stime, utimescaled, stimescaled, gtime);

    /*get task_io_accounting info*/
    uint64_t rchar = 0;
    uint64_t wchar = 0;
    uint64_t syscr = 0;
    uint64_t syscw = 0;
    uint64_t read_bytes = 0;
	uint64_t write_bytes = 0;
	uint64_t cancelled_write_bytes = 0;
    vmi_read_64_va(vmi, current_process + ioac_offset , 0, &rchar);
    vmi_read_64_va(vmi, current_process + ioac_offset + 8, 0, &wchar);
    vmi_read_64_va(vmi, current_process + ioac_offset + 16, 0, &syscr);
    vmi_read_64_va(vmi, current_process + ioac_offset + 24, 0, &syscw);
    vmi_read_64_va(vmi, current_process + ioac_offset + 32, 0, &read_bytes);
    vmi_read_64_va(vmi, current_process + ioac_offset + 40, 0, &write_bytes);
    vmi_read_64_va(vmi, current_process + ioac_offset + 48, 0, &cancelled_write_bytes);
    tmptn->tsrchar = rchar;
    tmptn->tswchar = wchar;
    tmptn->tssyscr = syscr;
    tmptn->tssyscw = syscw;
    tmptn->tsread_bytes = read_bytes;
    tmptn->tswrite_bytes = write_bytes;
    tmptn->tscancelled_write_bytes = cancelled_write_bytes;
    printf("rchar:%lu wchar:%lu syscr:%lu syscw:%lu read_bytes:%lu write_bytes:%lu cancelled_write_bytes:%lu\n", rchar, wchar, syscr, syscw, read_bytes, write_bytes, cancelled_write_bytes);

    vmi_read_addr_va(vmi, current_process + files_offset, 0, &files);
    if(VMI_FAILURE==vmi_read_addr_va(vmi, files + fdt_offset, 0, &fdt))
    {
        printf("%d %s Failed to read addr!!!!!\n",pid,procname);
    }
    vmi_read_addr_va(vmi, files + 40, 0, &fdtab);
    vmi_read_addr_va(vmi, files + 160, 0, &fd_array);
    vmi_read_32_va(vmi, fdt + maxfds_offset, 0, &max_fds);
    vmi_read_addr_va(vmi, fdt + fd_offset, 0, &fd);

    tmptn->tsfdnum = max_fds;

    // FdInfo tmpfdinfo;
    ///printf("tfb");

    /* show files info about this task_struct */
    get_files_info(vmi,fd,max_fds,tmptn);
    ///printf("tfa");
}///end get_task_info

#endif // MYPSINFO_H_INCLUDED
