/* The LibVMI Library is an introspection library that simplifies access to
 * memory in a target virtual machine or in a file containing a dump of
 * a system's physical memory.  LibVMI is based on the XenAccess Library.
 *
 * Copyright 2011 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government
 * retains certain rights in this software.
 *
 * Author: Bryan D. Payne (bdpayne@acm.org)
 *
 * This file is part of LibVMI.
 *
 * LibVMI is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * LibVMI is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with LibVMI.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdio.h>
#include <inttypes.h>

#include <libvmi/libvmi.h>


#include <linux/kernel.h>
#include <linux/sched.h>

#include <linux/version.h>



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


unsigned long fd_offset = 8;
unsigned long i_node_offset = 32;
unsigned long i_uid_offset = 4;
unsigned long i_gid_offset = 8;
unsigned long private_date_offset = 208;
unsigned long f_mode_offset = 68;

unsigned long socket_sk_offset = 32;
unsigned long sk_daddr_offset = 0;
unsigned long sk_rcv_saddr_offset = 4;
unsigned long sk_num_offset = 14;
unsigned long sk_dport_offset = 12;
unsigned long sk_v6_daddr_offet = 56;
unsigned long sk_v6_rcv_saddr_offet = 72;



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
unsigned fmode;


/**
*This funtion get the qstr name.
*/
char* get_qstr_name(vmi_instance_t vmi,addr_t qstr_addr)
{
    char qstr_d_name[255]="";
    char c = 1;
    //while(c != NULL && filetype != 0)
    printf("??");
    while(c != NULL)
    {
        vmi_read_8_va(vmi, qstr_addr, 0, &c);
        qstr_addr = qstr_addr + nadd;
        //printf("%c",c);
        char c_rmp[2]= {c};
        strcat(qstr_d_name,c_rmp);

    }
    // printf("qstr:%s ",qstr_d_name);
    return qstr_d_name;

}

/**
*This funtion get the dentry qstr name.
*/
char* get_dentry_name(vmi_instance_t vmi,addr_t dentry_addr)
{
    char qstr_d_name[255]="";
    char c = 1;
    vmi_read_addr_va(vmi, dentry_addr + dname_offset + qname_offset, 0, &p_name);
    //while(c != NULL && filetype != 0)
    while(c != NULL)
    {
        vmi_read_8_va(vmi, p_name, 0, &c);
        p_name = p_name + nadd;
        // printf("%c",c);
        char c_rmp[2]= {c};
        strcat(qstr_d_name,c_rmp);

    }
    // printf("qstr:%s ",qstr_d_name);
    return qstr_d_name;

}

/**
*This funtion for show the relevant files infomations.
*Include files socket and so on.
*/
void get_files_info(vmi_instance_t vmi,addr_t fdaddr,unsigned int max_file)
{
    int i;

    for(i=0; i<max_file; i++)
    {

        vmi_read_addr_va(vmi, fdaddr, 0, &file);
        fdaddr = fdaddr + fdadd;
        if(file!=0)
        {
            printf("%3d  ",i);
            int filetype = 50;
            //vmi_read_addr_va(vmi, file + 16, 0, &f_path);
            vmi_read_addr_va(vmi, file + path_offset+dentry_offset, 0, &dentry1);
            vmi_read_addr_va(vmi, file + path_offset, 0, &mnt);
            vmi_read_addr_va(vmi, file + i_node_offset, 0, &i_node);

            /* read the fmode */
//            vmi_read_32_va(vmi, file + f_mode_offset, 0, &fmode);
//            int j = 0;
//            printf("rwx:%lu ",fmode);
//            unsigned int flag = 1;
//            for(;j<32;j++){
//                if(fmode&(flag<<j)){
//                    printf("1");
//                }
//                else{
//                    printf("0");
//                }
//
//            }
//            printf(" ");


            vmi_read_addr_va(vmi, dentry1 + dname_offset, 0, &d_name);
            //unsigned char *xname = vmi_read_str_va(vmi, dentry1 + 40, 0);
            vmi_read_addr_va(vmi, dentry1 + dname_offset + qname_offset, 0, &f_name);

            unsigned int kuid,kgid;
            vmi_read_32_va(vmi, i_node + i_uid_offset, 0, &kuid);
            vmi_read_32_va(vmi, i_node + i_gid_offset, 0, &kgid);
            printf(" ku:gid([%4d]:[%4d]) ",kuid,kgid);


            vmi_read_addr_va(vmi, mnt, 0, &droot);
            vmi_read_addr_va(vmi, droot + dname_offset + qname_offset, 0, &rname);

//            char vfsname[50]="";
//            char ao='1';
//            while(ao!=NULL)
//            {
//                vmi_read_8_va(vmi, rname, 0, &ao);
//                rname = rname + nadd;
//                // printf("%c",ao);
//                char ap[2]= {ao};
//                strcat(vfsname,ap);
//            }

            // char *vfsname = get_dentry_name(vmi,droot);
            char vfsname[255]="";
            strcat(vfsname,get_dentry_name(vmi,droot));
            if(vfsname[0]!='/')
            {
                filetype=0;
                printf("vn:%s",vfsname);
            }

            char a = '1';
            int tcp = 1;
            char filename[255]="";
            strcat(filename,get_dentry_name(vmi,dentry1));
            printf("%s",filename);
            if(strcmp(filename,"TCPv6")==0)
            {
                tcp = 0;
            }
//            while(a!=NULL)
//            {
//                vmi_read_8_va(vmi,f_name,0,&a);
//                f_name = f_name + nadd;
//                printf("%c",a);
//                if(a == '6')
//                {
//                    tcp = 0;
//                }
//            }
            if(vfsname[0] != '/'&&tcp == 1)
            {
                vmi_read_addr_va(vmi, file + private_date_offset, 0, &private_date);
                vmi_read_addr_va(vmi, private_date + socket_sk_offset, 0, &sk);
                vmi_read_32_va(vmi, sk + sk_daddr_offset, 0, &daddr);
                vmi_read_32_va(vmi, sk + sk_rcv_saddr_offset, 0, &rcv_saddr);
                unsigned char *rcvIp = &rcv_saddr;
                unsigned char *dIp = &daddr;
                vmi_read_16_va(vmi, sk + sk_num_offset, 0, &sk_num);
                vmi_read_16_va(vmi, sk + sk_dport_offset, 0, &sk_dport);
                // printf("local:%x(num:%x)  source:%x(port:%x)",rcv_saddr,sk_num,daddr,sk_dport);
                printf(" %d.%d.%d.%d:%d <-- ",rcvIp[0],rcvIp[1],rcvIp[2],rcvIp[3],sk_num);
                printf("%d.%d.%d.%d:%d ",dIp[0],dIp[1],dIp[2],dIp[3],sk_dport);
            }
            if(tcp == 0)
            {
                vmi_read_addr_va(vmi, file + private_date_offset, 0, &private_date);
                vmi_read_addr_va(vmi, private_date + socket_sk_offset, 0, &sk);
                vmi_read_64_va(vmi, sk + sk_v6_daddr_offet, 0, &v6_daddr);
                vmi_read_64_va(vmi, sk + sk_v6_rcv_saddr_offet, 0, &v6_rcv_saddr);
                unsigned char *v6dIp = &v6_daddr;
                unsigned char *v6rcvIp = &v6_rcv_saddr;

                vmi_read_16_va(vmi, sk + sk_num_offset, 0, &sk_num);
                vmi_read_16_va(vmi, sk + sk_dport_offset, 0, &sk_dport);
                // printf(" %x: <-- ",v6_rcv_saddr);
                printf(" %x%02x:%x%02x:%x%02x:%x%02x :%d <-- ",v6rcvIp[0],v6rcvIp[1],v6rcvIp[2],v6rcvIp[3],v6rcvIp[4],v6rcvIp[5],v6rcvIp[6],v6rcvIp[7],sk_num);
                printf("%x%02x:%x%02x:%x%02x:%x%02x :%d ",v6dIp[0],v6dIp[1],v6dIp[2],v6dIp[3],v6dIp[4],v6dIp[5],v6dIp[6],v6dIp[7],sk_dport);
            }
            printf("|| ");
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
                strcat(parentname,get_dentry_name(vmi,d_parent));
                printf("%s",parentname);
                if(strcmp(parentname,"/")==0)
                {
                    filetype = 0;
                }
//                while(a != NULL && filetype != 0)
//                {
//                    vmi_read_8_va(vmi, p_name, 0, &a);
//                    p_name = p_name + nadd;
//                    printf("%c",a);
//                    if(a=='/')
//                    {
//                        filetype = 0;
//                    }
//                }
                printf("  ");
                vmi_read_addr_va(vmi, d_parent + parent_offset, 0, &d_parent);
            }
            printf("|| \n");

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
}

/**
*4.4.57 nameidata is null...
*/
void get_nameidata_info(vmi_instance_t vmi, addr_t nameidat)
{
    addr_t namei_path = nameidat + nameidata_path_offset;
    addr_t namei_last = nameidat + nameidata_last_offset;
    addr_t namei_root = nameidat + nameidata_root_offset;


    char nameipath[255]="";
    // get_dentry_name(vmi,namei_path+dentry_offset);
    strcat(nameipath,get_dentry_name(vmi,namei_path+dentry_offset));
    printf("nameipath:%s  ",nameipath);


    char nameilast[255]="";
    //get_qstr_name(vmi,namei_last+8);
    strcat(nameilast,get_qstr_name(vmi,namei_last+qname_offset));
    printf("nameilast:%s  ",nameilast);


    char nameiroot[255]="";
    strcat(nameiroot,get_dentry_name(vmi,namei_root+dentry_offset));
    printf("nameipath:%s\n",nameiroot);


}
/**
* show fs information
* only pwd is not null, fs other is null
*/
void get_fs_info(vmi_instance_t vmi, addr_t fs)
{
    vmi_read_addr_va(vmi, fs + fsroot_offset + dentry_offset, 0, &fsroot_dentry);
    vmi_read_addr_va(vmi, fs + fspwd_offset + dentry_offset, 0, &fspwd_dentry);
    if(fsroot_dentry != NULL&& fspwd_dentry != NULL)
    {
        addr_t vfs_mnt = 0;
        addr_t mnt_dentry = 0;
        vmi_read_addr_va(vmi, fs + fsroot_offset, 0, &vfs_mnt);
        vmi_read_addr_va(vmi, vfs_mnt, 0, &mnt_dentry);
        char rootvfsdentry[255]="";
        strcat(rootvfsdentry,get_dentry_name(vmi,mnt_dentry));
        printf("rootvfs:%s ",rootvfsdentry);

        char rootdentry[255]="";
        strcat(rootdentry,get_dentry_name(vmi,fsroot_dentry));
        printf("root:%s ",rootdentry);


        vmi_read_addr_va(vmi, fs + fspwd_offset, 0, &vfs_mnt);
        vmi_read_addr_va(vmi, vfs_mnt, 0, &mnt_dentry);
        char pwdvfsdentry[255]="";
        strcat(pwdvfsdentry,get_dentry_name(vmi,mnt_dentry));
        printf("pwdvfs:%s ",pwdvfsdentry);


        char pwddentrystr[255]="";
        strcat(pwddentrystr,get_dentry_name(vmi,fspwd_dentry));
        int n = 3;
        while(n > 0 && pwddentrystr[0] != '/')
        {
            //n = n - 1;
            char pwddentry[255]="";
            vmi_read_addr_va(vmi, fspwd_dentry + parent_offset, 0, &fspwd_dentry);
            strcat(pwddentrystr,"/");
            strcat(pwddentry,get_dentry_name(vmi,fspwd_dentry));
            if(pwddentry[0] == '/')
            {
                n = 0;
            }
            else
            {
                strcat(pwddentrystr,pwddentry);
            }
        }
        printf("pwd:%s\n",pwddentrystr);
    }
}

int main (int argc, char **argv)
{
    vmi_instance_t vmi;
    addr_t list_head = 0, next_list_entry = 0;
    addr_t current_process = 0;
    char *procname = NULL;
    vmi_pid_t pid = 0;
    vmi_pid_t real_parent_pid = 0;
    vmi_pid_t parent_pid = 0;
    vmi_pid_t tgid = 0;
    vmi_pid_t group_leader_pid = 0;

    unsigned long tasks_offset = 0, pid_offset = 0, name_offset = 0;
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

    /* this is the VM or file that we are looking at */
    if (argc != 2)
    {
        printf("Usage: %s <vmname>\n", argv[0]);
        return 1;
    } // if

    char *name = argv[1];

    /* initialize the libvmi library */
    if (VMI_FAILURE ==
            vmi_init_complete(&vmi, name, VMI_INIT_DOMAINNAME, NULL,
                              VMI_CONFIG_GLOBAL_FILE_ENTRY, NULL, NULL))
    {
        printf("Failed to init LibVMI library.\n");
        return 1;
    }

    /* init the offset values */
    if (VMI_OS_LINUX == vmi_get_ostype(vmi))
    {
        tasks_offset = vmi_get_offset(vmi, "linux_tasks");
        name_offset = vmi_get_offset(vmi, "linux_name");
        pid_offset = vmi_get_offset(vmi, "linux_pid");
    }
    else if (VMI_OS_WINDOWS == vmi_get_ostype(vmi))
    {
        tasks_offset = vmi_get_offset(vmi, "win_tasks");
        name_offset = vmi_get_offset(vmi, "win_pname");
        pid_offset = vmi_get_offset(vmi, "win_pid");
    }

    if (0 == tasks_offset)
    {
        printf("Failed to find win_tasks\n");
        goto error_exit;
    }
    if (0 == pid_offset)
    {
        printf("Failed to find win_pid\n");
        goto error_exit;
    }
    if (0 == name_offset)
    {
        printf("Failed to find win_pname\n");
        goto error_exit;
    }

    /* pause the vm for consistent memory access */
    if (vmi_pause_vm(vmi) != VMI_SUCCESS)
    {
        printf("Failed to pause VM\n");
        goto error_exit;
    } // if

    /* demonstrate name and id accessors */
    char *name2 = vmi_get_name(vmi);
    vmi_mode_t mode;

    if (VMI_FAILURE == vmi_get_access_mode(vmi, NULL, 0, NULL, &mode))
        goto error_exit;

    if ( VMI_FILE != mode )
    {
        uint64_t id = vmi_get_vmid(vmi);

        printf("Process listing for VM %s (id=%"PRIu64")\n", name2, id);
    }
    else
    {
        printf("Process listing for file %s\n", name2);
    }
    free(name2);

    /* get the head of the list */
    if (VMI_OS_LINUX == vmi_get_ostype(vmi))
    {
        /* Begin at PID 0, the 'swapper' task. It's not typically shown by OS
         *  utilities, but it is indeed part of the task list and useful to
         *  display as such.
         */
        list_head = vmi_translate_ksym2v(vmi, "init_task") + tasks_offset;
    }
    else if (VMI_OS_WINDOWS == vmi_get_ostype(vmi))
    {

        // find PEPROCESS PsInitialSystemProcess
        if(VMI_FAILURE == vmi_read_addr_ksym(vmi, "PsActiveProcessHead", &list_head))
        {
            printf("Failed to find PsActiveProcessHead\n");
            goto error_exit;
        }
    }

    next_list_entry = list_head;

    /* walk the task list */
    do
    {

        current_process = next_list_entry - tasks_offset;
        //printf("current_process:%lu\n",current_process);

        /* Note: the task_struct that we are looking at has a lot of
         * information.  However, the process name and id are burried
         * nice and deep.  Instead of doing something sane like mapping
         * this data to a task_struct, I'm just jumping to the location
         * with the info that I want.  This helps to make the example
         * code cleaner, if not more fragile.  In a real app, you'd
         * want to do this a little more robust :-)  See
         * include/linux/sched.h for mode details */

        /* NOTE: _EPROCESS.UniqueProcessId is a really VOID*, but is never > 32 bits,
         * so this is safe enough for x64 Windows for example purposes */
        vmi_read_32_va(vmi, current_process + pid_offset, 0, (uint32_t*)&pid);
        vmi_read_32_va(vmi, current_process + tgid_offset, 0, (uint32_t*)&tgid);

        vmi_read_addr_va(vmi, current_process + task_real_parent_offset, 0, &task_real_parent);
        vmi_read_32_va(vmi, task_real_parent + pid_offset, 0, (uint32_t*)&real_parent_pid);
        vmi_read_addr_va(vmi, current_process + task_parent_offset, 0, &task_parent);
        vmi_read_32_va(vmi, task_parent + pid_offset, 0, (uint32_t*)&parent_pid);

        procname = vmi_read_str_va(vmi, current_process + name_offset, 0);

        if (!procname)
        {
            printf("Failed to find procname\n");
            goto error_exit;
        }

        /* print out the process name */
        printf("\n[%5d] [%5d] %s (struct addr:%"PRIx64")\n   rparent:%d  parent:%d ", pid, tgid, procname, current_process, real_parent_pid, parent_pid);


        vmi_read_addr_va(vmi, current_process + group_leader_offset, 0, &group_leader);
        vmi_read_32_va(vmi, group_leader + pid_offset, 0, (uint32_t*)&group_leader_pid);
        printf("groupleader:%d ",group_leader_pid);

        /* show fs information */
        vmi_read_addr_va(vmi, current_process + fs_offset, 0, &fs);
        get_fs_info(vmi, fs);

        /* show start time */
        uint64_t starttime = 0;
        vmi_read_64_va(vmi, current_process + start_time_offset, 0, (uint64_t*)&starttime);
        uint64_t realstarttime = 0;
        vmi_read_64_va(vmi, current_process + real_start_time_offset, 0, &realstarttime);
        printf("starttime:%u  realstarttime:%u ",starttime,realstarttime);

        /* show mm fault and swap info */
        unsigned long minflt = 0;
        vmi_read_64_va(vmi, current_process + min_flt_offset, 0, &minflt);
        unsigned long majflt = 0;
        vmi_read_64_va(vmi, current_process + maj_flt_offset, 0, &majflt);
        printf("minflt:%u majflt:%u \n", minflt, majflt);

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
        vmi_read_64_va(vmi, current_process + stime_offset + 24, 0, &gtime);
        printf("utime:%u stime:%u utimescaled:%u stimescaled:%u gtime:%u\n", utime, stime, utimescaled, stimescaled, gtime);






//        vmi_read_addr_va(vmi, current_process + nameidata_offset, 0, &nameida);
//        /* show nameidata info */
//        if(nameida!=NULL)
//        {
//            get_nameidata_info(vmi,nameida);
//        }
//        else
//        {
//            printf("nameidata is NULL!\n");
//        }


        vmi_read_addr_va(vmi, current_process + files_offset, 0, &files);
        if(VMI_FAILURE==vmi_read_addr_va(vmi, files + fdt_offset, 0, &fdt))
        {
            printf("Failed to read addr!!!!!\n");
        }
        vmi_read_addr_va(vmi, files + 40, 0, &fdtab);
        vmi_read_addr_va(vmi, files + 160, 0, &fd_array);
        vmi_read_32_va(vmi, fdt + maxfds_offset, 0, &max_fds);
        vmi_read_addr_va(vmi, fdt + fd_offset, 0, &fd);


        /* show files info about this task_struct */
        get_files_info(vmi,fd,max_fds);




        if (procname)
        {
            free(procname);
            procname = NULL;
        }

        /* follow the next pointer */

        status = vmi_read_addr_va(vmi, next_list_entry, 0, &next_list_entry);
        if (status == VMI_FAILURE)
        {
            printf("Failed to read next pointer in loop at %"PRIx64"\n", next_list_entry);
            goto error_exit;
        }

    }
    while(next_list_entry != list_head);

error_exit:
    /* resume the vm */
    vmi_resume_vm(vmi);

    /* cleanup any memory associated with the LibVMI instance */
    vmi_destroy(vmi);

    return 0;
}
