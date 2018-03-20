/**
* Copyright(c) 2017 dyf
* record task_struct/files/fd/sk/dentry... offset
* give some funtions to get infomation about one task deeply
*/

#include "mypsinfo.h"
#include "myList.h"

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

addr_t fs = 1;

addr_t files = 1;
addr_t fdt = 1;
addr_t fdtab = 1;
addr_t fd_array = 1;
addr_t fd = 1;

unsigned int max_fds = 0;
//char *filename = NULL;
char *d_iname = NULL;
//char *parentname = NULL;

/**
 * This funtion get mm_struct info
 */
void get_mm_struct(VmiInfo* vmiinfo, addr_t currentps, TaskNode *tmptn)
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
    vmi_read_addr_va(vmiinfo->vmi, currentps + vmiinfo->vmoffset[mm_offset], 0, &currmm_structaddr);
    vmi_read_32_va(vmiinfo->vmi, currmm_structaddr + vmiinfo->vmoffset[mm_users_offset], 0, &cmm_users);
    vmi_read_32_va(vmiinfo->vmi, currmm_structaddr + vmiinfo->vmoffset[mm_count_offset], 0, &cmm_count);
    vmi_read_64_va(vmiinfo->vmi, currmm_structaddr + vmiinfo->vmoffset[pgd_offset], 0, &cpgd);
    vmi_read_64_va(vmiinfo->vmi, currmm_structaddr + vmiinfo->vmoffset[total_vm_offset], 0, &ctotal_vm);
    vmi_read_64_va(vmiinfo->vmi, currmm_structaddr + vmiinfo->vmoffset[locked_vm_offset], 0, &clocked_cm);
    vmi_read_64_va(vmiinfo->vmi, currmm_structaddr + vmiinfo->vmoffset[stack_vm_offset], 0, &cstack_vm);
    vmi_read_64_va(vmiinfo->vmi, currmm_structaddr + vmiinfo->vmoffset[start_code_offset], 0, &cstart_code);
    vmi_read_64_va(vmiinfo->vmi, currmm_structaddr + vmiinfo->vmoffset[end_code_offset], 0, &cend_code);
    tmptn->tsmm_users = cmm_users;
    tmptn->tsmm_count = cmm_count;
    if (cmm_count == 0)
    {
        tmptn->tsmm_users = 0;
    }
    tmptn->tstotal_vm = ctotal_vm;
    tmptn->tsstack_vm = cstack_vm;
    /**stack_vm to large may no data or system space*/
    if (cstack_vm > 5000)
    {
        tmptn->tsstack_vm = 0;
    }
    ///printf("mm_users:%d mm_count:%d pgd:%p total_vm:%ld locked_cm:%ld stack_vm:%ld start_code:%p end_code:%p\n",cmm_users,cmm_count,(void*)cpgd,ctotal_vm,clocked_cm,cstack_vm,(void*)cstart_code,(void*)cend_code);
}

/**
*This funtion get the dentry qstr name.
*/
char *get_dentry_name(VmiInfo* vmiinfo, char *name, addr_t dentry_addr)
{
    char qstr_d_name[255] = "";
    //qstr_d_name = (char*)malloc(255);
    char c = 1;
    vmi_read_addr_va(vmiinfo->vmi, dentry_addr + vmiinfo->vmoffset[dname_offset] + vmiinfo->vmoffset[qname_offset], 0, &p_name);
    while (c != '\0')
    {
        vmi_read_8_va(vmiinfo->vmi, p_name, 0, &c);
        p_name = p_name + nadd;
        // printf("%c",c);
        char c_rmp[2] = {c};
        strcat(qstr_d_name, c_rmp);
    }
    // printf("qstr:%s ",qstr_d_name);
    strcpy(name, qstr_d_name);
    return name;

} ///end get_dentry_name

/**
*This funtion for show the relevant files infomations.
*Include files socket and so on.
*/
void get_files_info(VmiInfo* vmiinfo, addr_t fdaddr, unsigned int max_file, TaskNode *tmp)
{
    int i;
    tmp->tsfdnum = 0;
    MyList * socket_list= createMyList();
    MyList * file_list= createMyList();

    for (i = 0; i < max_file; i++)
    {
        vmi_read_addr_va(vmiinfo->vmi, fdaddr, 0, &file);
        fdaddr = fdaddr + fdadd;
        if (file != 0)
        {
            tmp->tsfdnum = tmp->tsfdnum + 1;
            ///printf("%3d",i);
            int filetype = 50;
            //vmi_read_addr_va(vmiinfo->vmi, file + 16, 0, &f_path);
            vmi_read_addr_va(vmiinfo->vmi, file + vmiinfo->vmoffset[path_offset] + vmiinfo->vmoffset[dentry_offset], 0, &dentry1);
            vmi_read_addr_va(vmiinfo->vmi, file + vmiinfo->vmoffset[path_offset], 0, &mnt);
            vmi_read_addr_va(vmiinfo->vmi, file + vmiinfo->vmoffset[i_node_offset], 0, &i_node);

            /* read flags*/
            vmi_read_32_va(vmiinfo->vmi, file + vmiinfo->vmoffset[f_flags_offset], 0, &flags);
            ///printf("flag:%u ",flags);

            /* read the fmode */
            vmi_read_32_va(vmiinfo->vmi, file + vmiinfo->vmoffset[f_mode_offset], 0, &fmode);
            int j = 0;
            ///printf("fmode:%u ",fmode);

            vmi_read_addr_va(vmiinfo->vmi, dentry1 + vmiinfo->vmoffset[dname_offset], 0, &d_name);
            //unsigned char *xname = vmi_read_str_va(vmiinfo->vmi, dentry1 + 40, 0);
            vmi_read_addr_va(vmiinfo->vmi, dentry1 + vmiinfo->vmoffset[dname_offset] + vmiinfo->vmoffset[qname_offset], 0, &f_name);

            uint32_t imode;
            unsigned int kuid, kgid;
            vmi_read_32_va(vmiinfo->vmi, i_node, 0, &imode);
            vmi_read_32_va(vmiinfo->vmi, i_node + vmiinfo->vmoffset[i_uid_offset], 0, &kuid);
            vmi_read_32_va(vmiinfo->vmi, i_node + vmiinfo->vmoffset[i_gid_offset], 0, &kgid);
            ///printf("flag:%u fmode:%u imode:%u ku:gid([%4d]:[%4d]) ",flags,fmode,imode,kuid,kgid);

            vmi_read_addr_va(vmiinfo->vmi, mnt, 0, &droot);
            vmi_read_addr_va(vmiinfo->vmi, droot + vmiinfo->vmoffset[dname_offset] + vmiinfo->vmoffset[qname_offset], 0, &rname);

            //char vfsname[255] = "";
            //get_dentry_name(vmiinfo, vfsname, droot);
            vmi_read_addr_va(vmiinfo->vmi, droot + vmiinfo->vmoffset[dname_offset] + vmiinfo->vmoffset[qname_offset], 0, &p_name);
            char* vfsname = vmi_read_str_va(vmiinfo->vmi,p_name,0);
            vmi_read_addr_va(vmiinfo->vmi, dentry1 + vmiinfo->vmoffset[dname_offset] + vmiinfo->vmoffset[qname_offset], 0, &p_name);
            char* filename = vmi_read_str_va(vmiinfo->vmi,p_name,0);
            //char filename[255] = "";

            //strcat(vfsname,get_dentry_name(vmiinfo,droot));
            if (vfsname[0] != '/')
            {
                filetype = 0;
                //printf("vn:%s ",vfsname); 
            }
            else
            {
                tmp->fileinfo = tmp->fileinfo + 1;
            }

            char a = '1';
            int tcp = 1;
            
            //strcat(filename,get_dentry_name(vmiinfo,dentry1));
            //get_dentry_name(vmiinfo, filename, dentry1);
            ///printf("fs:%s",filename);

            if (strcmp(vfsname, "pipe:") == 0)
                tmp->pipeinfo = tmp->pipeinfo + 1;
            if (strcmp(vfsname, "\0") == 0)
                tmp->nullinfo = tmp->nullinfo + 1;
            if (strcmp(filename, "TCPv6") == 0)
            {
                tcp = 0;
                tmp->socketinfo[3] = tmp->socketinfo[3] + 1;
            }

            if (vfsname[0] != '/' && tcp == 1)
            {
                if (strcmp(filename, "UNIX") == 0)
                    tmp->socketinfo[0] = tmp->socketinfo[0] + 1;
                if (strcmp(filename, "NETLINK") == 0)
                    tmp->socketinfo[1] = tmp->socketinfo[1] + 1;
                if (strcmp(filename, "[eventfd]") == 0)
                    tmp->anon_inodeinfo[0] = tmp->anon_inodeinfo[0] + 1;
                if (strcmp(filename, "inotify") == 0)
                    tmp->anon_inodeinfo[1] = tmp->anon_inodeinfo[1] + 1;
                if (strcmp(filename, "[timerfd]") == 0)
                    tmp->anon_inodeinfo[2] = tmp->anon_inodeinfo[2] + 1;
                if (strcmp(filename, "[signalfd]") == 0)
                    tmp->anon_inodeinfo[3] = tmp->anon_inodeinfo[3] + 1;
                if (strcmp(filename, "[eventpoll]") == 0)
                    tmp->anon_inodeinfo[4] = tmp->anon_inodeinfo[4] + 1;
                vmi_read_addr_va(vmiinfo->vmi, file + vmiinfo->vmoffset[private_date_offset], 0, &private_date);
                vmi_read_addr_va(vmiinfo->vmi, private_date + vmiinfo->vmoffset[socket_sk_offset], 0, &sk);
                vmi_read_32_va(vmiinfo->vmi, sk + vmiinfo->vmoffset[sk_daddr_offset], 0, &daddr);
                vmi_read_32_va(vmiinfo->vmi, sk + vmiinfo->vmoffset[sk_rcv_saddr_offset], 0, &rcv_saddr);
                unsigned char *rcvIp = (unsigned char *)&rcv_saddr;
                unsigned char *dIp = (unsigned char *)&daddr;
                vmi_read_16_va(vmiinfo->vmi, sk + vmiinfo->vmoffset[sk_num_offset], 0, &sk_num);
                vmi_read_16_va(vmiinfo->vmi, sk + vmiinfo->vmoffset[sk_dport_offset], 0, &sk_dport);

                if (strcmp(filename, "TCP") == 0){
                    tmp->socketinfo[2] = tmp->socketinfo[2] + 1;
                    SocketSR *sock = malloc(sizeof(SocketSR));
                    sock->classify = 1;
                    sock->sendport = sk_dport;
                    sock->recieveport = sk_num;
                    sprintf(sock->recvaddr,"%d.%d.%d.%d:%d",rcvIp[0],rcvIp[1],rcvIp[2],rcvIp[3]);
                    sprintf(sock->sendaddr,"%d.%d.%d.%d:%d",dIp[0],dIp[1],dIp[2],dIp[3]);
                    myListInsertDataAtLast(socket_list, sock);
                }
                if (strcmp(filename, "UDP") == 0){
                    tmp->socketinfo[3] = tmp->socketinfo[3] + 1;
                    SocketSR *sock = malloc(sizeof(SocketSR));
                    sock->classify = 2;
                    sock->sendport = sk_dport;
                    sock->recieveport = sk_num;
                    sprintf(sock->recvaddr,"%d.%d.%d.%d:%d",rcvIp[0],rcvIp[1],rcvIp[2],rcvIp[3]);
                    sprintf(sock->sendaddr,"%d.%d.%d.%d:%d",dIp[0],dIp[1],dIp[2],dIp[3]);
                    myListInsertDataAtLast(socket_list, sock);
                }
                ///printf("local:%d(num:%d)  source:%d(port:%d)",rcv_saddr,sk_num,daddr,sk_dport);
                ///printf(" %d.%d.%d.%d:%d <--- ",rcvIp[0],rcvIp[1],rcvIp[2],rcvIp[3],sk_num);
                ///printf("%d.%d.%d.%d:%d ",dIp[0],dIp[1],dIp[2],dIp[3],sk_dport);
            }
            if (tcp == 0)
            {
                vmi_read_addr_va(vmiinfo->vmi, file + vmiinfo->vmoffset[private_date_offset], 0, &private_date);
                vmi_read_addr_va(vmiinfo->vmi, private_date + vmiinfo->vmoffset[socket_sk_offset], 0, &sk);
                vmi_read_64_va(vmiinfo->vmi, sk + vmiinfo->vmoffset[sk_v6_daddr_offset], 0, &v6_daddr);
                vmi_read_64_va(vmiinfo->vmi, sk + vmiinfo->vmoffset[sk_v6_rcv_saddr_offset], 0, &v6_rcv_saddr);
                unsigned char *v6dIp = (unsigned char *)&v6_daddr;
                unsigned char *v6rcvIp = (unsigned char *)&v6_rcv_saddr;

                vmi_read_16_va(vmiinfo->vmi, sk + vmiinfo->vmoffset[sk_num_offset], 0, &sk_num);
                vmi_read_16_va(vmiinfo->vmi, sk + vmiinfo->vmoffset[sk_dport_offset], 0, &sk_dport);

                SocketSR *sock = malloc(sizeof(SocketSR));
                sock->classify = 1;
                sock->sendport = sk_dport;
                sock->recieveport = sk_num;
                sprintf(sock->recvaddr,"%x%02x:%x%02x:%x%02x:%x%02x",v6rcvIp[0],v6rcvIp[1],v6rcvIp[2],v6rcvIp[3],v6rcvIp[4],v6rcvIp[5],v6rcvIp[6],v6rcvIp[7]);
                sprintf(sock->sendaddr,"%x%02x:%x%02x:%x%02x:%x%02x",v6dIp[0],v6dIp[1],v6dIp[2],v6dIp[3],v6dIp[4],v6dIp[5],v6dIp[6],v6dIp[7]);
                myListInsertDataAtLast(socket_list, sock);
                ///printf(" %lx: <-- %lx ",v6_rcv_saddr,v6_daddr);
                ///printf(" %x%02x:%x%02x:%x%02x:%x%02x :%d <-- ",v6rcvIp[0],v6rcvIp[1],v6rcvIp[2],v6rcvIp[3],v6rcvIp[4],v6rcvIp[5],v6rcvIp[6],v6rcvIp[7],sk_num);
                ///printf("%x%02x:%x%02x:%x%02x:%x%02x :%d ",v6dIp[0],v6dIp[1],v6dIp[2],v6dIp[3],v6dIp[4],v6dIp[5],v6dIp[6],v6dIp[7],sk_dport);
            }
            ///printf("|| ");
            vmi_read_addr_va(vmiinfo->vmi, dentry1 + vmiinfo->vmoffset[parent_offset], 0, &d_parent);
            //printf(" %x end",d_parent);
            a = 1;
            char pa = '1';
            int n = 0;

            char filepath[255] = "";
            while (d_parent != 0 && n < filetype)
            {
                n = n + 1;
                // vmi_read_addr_va(vmiinfo->vmi, d_parent + vmiinfo->vmoffset[dname_offset] + vmiinfo->vmoffset[qname_offset], 0, &p_name);
                // a=1;
                //char parentname[255] = "";
                //get_dentry_name(vmiinfo, parentname, d_parent);

                vmi_read_addr_va(vmiinfo->vmi, d_parent + vmiinfo->vmoffset[dname_offset] + vmiinfo->vmoffset[qname_offset], 0, &p_name);
                char* parentname = vmi_read_str_va(vmiinfo->vmi,p_name,0);
                char cparentname[255] = "";
                strcpy(cparentname,parentname);
                //printf("%s",parentname);
                
                if (strcmp(cparentname, "/") == 0)
                {
                    filetype = 0;
                }
                else
                {
                    strcat(cparentname,filepath);
                    strcpy(filepath,"/");
                    strcat(filepath,cparentname);
                }
                free(parentname);
                vmi_read_addr_va(vmiinfo->vmi, d_parent + vmiinfo->vmoffset[parent_offset], 0, &d_parent);
            }
            char cvfsname[255] = "";
            strcpy(cvfsname,vfsname);
            free(vfsname);
            if(cvfsname[0]=='/'){
                strcat(filepath,"/");
            }
            else
            {
                strcat(cvfsname,filepath);
                strcpy(filepath,cvfsname);
            }
            FileFD* filefd = malloc(sizeof(FileFD));
            filefd->fd = i;
            strcat(filepath,filename);
            strcpy(filefd->filepath,filepath);
            myListInsertDataAtLast(file_list, filefd);
            
            free(filename);
            //printf("%s||%d \n",filefd->filepath,n);

            ////////////////////////////////////////////////////
            /*small with dentry qstr name.
            addr_t di_name = dentry1 + vmiinfo->vmoffset[di_name_offset];
            a = '1';
            while(a!=NULL){
                vmi_read_8_va(vmiinfo->vmi,di_name,0,&a);
                di_name = di_name + nadd;
                printf("%c",a);
            }
            printf(" \n----------\n");
            */
            ///////////////////////////////////////////////////////////////
        }
    }
    tmp->socket_list = socket_list;
    tmp->file_list = file_list;
    //fdinfoaddr = &tmp;
} ///end get_files_info

/**
*This funtion get the qstr name.
*/
char *get_qstr_name(VmiInfo* vmiinfo, char *name, addr_t qstr_addr)
{
    char qstr_name[255] = "";
    //qstr_name = (char*)malloc(255);
    //qstr_d_name[0] = '';
    char c = 1;
    ///printf("??");
    while (c != '\0')
    {
        vmi_read_8_va(vmiinfo->vmi, qstr_addr, 0, &c);
        qstr_addr = qstr_addr + nadd;
        //printf("%c",c);
        char c_rmp[2] = {c};
        strcat(qstr_name, c_rmp);
    }
    strcpy(name, qstr_name);
    // printf("qstr:%s ",qstr_d_name);
    return name;
} ///end get_qstr_name

/**
*4.4.57 nameidata is null...no use
*/
void get_nameidata_info(VmiInfo* vmiinfo, addr_t nameidat)
{
    addr_t namei_path = nameidat + vmiinfo->vmoffset[nameidata_path_offset];
    addr_t namei_last = nameidat + vmiinfo->vmoffset[nameidata_last_offset];
    addr_t namei_root = nameidat + vmiinfo->vmoffset[nameidata_root_offset];

    //char *getname;
    char nameipath[255] = "";
    // get_dentry_name(vmiinfo,namei_path + vmiinfo->vmoffset[dentry_offset]);
    get_dentry_name(vmiinfo, nameipath, namei_path + vmiinfo->vmoffset[dentry_offset]);
    printf("nameipath:%s  ", nameipath);

    char nameilast[255] = "";
    //get_qstr_name(vmiinfo,namei_last+8);
    get_qstr_name(vmiinfo, nameilast, namei_last + vmiinfo->vmoffset[qname_offset]);

    printf("nameilast:%s  ", nameilast);

    char nameiroot[255] = "";
    get_dentry_name(vmiinfo, nameiroot, namei_root + vmiinfo->vmoffset[dentry_offset]);
    printf("nameipath:%s\n", nameiroot);

} ///end get_nameidata_info

/**
* show fs information
* only pwd is not null, fs other is null
*/
void get_fs_info(VmiInfo* vmiinfo, addr_t fs)
{
    vmi_read_addr_va(vmiinfo->vmi, fs + vmiinfo->vmoffset[fsroot_offset] + vmiinfo->vmoffset[dentry_offset], 0, &fsroot_dentry);
    vmi_read_addr_va(vmiinfo->vmi, fs + vmiinfo->vmoffset[fspwd_offset] + vmiinfo->vmoffset[dentry_offset], 0, &fspwd_dentry);
    if (fsroot_dentry != 0 && fspwd_dentry != 0) //!!!!!befor != NULL
    {
        addr_t vfs_mnt = 0;
        addr_t mnt_dentry = 0;
        vmi_read_addr_va(vmiinfo->vmi, fs + vmiinfo->vmoffset[fsroot_offset], 0, &vfs_mnt);
        vmi_read_addr_va(vmiinfo->vmi, vfs_mnt, 0, &mnt_dentry);
        char rootvfsdentry[255] = "";
        get_dentry_name(vmiinfo, rootvfsdentry, mnt_dentry);
        ///printf("rootvfs:%s ",rootvfsdentry);

        char rootdentry[255] = "";
        get_dentry_name(vmiinfo, rootdentry, fsroot_dentry);
        ///printf("root:%s ",rootdentry);

        vmi_read_addr_va(vmiinfo->vmi, fs + vmiinfo->vmoffset[fspwd_offset], 0, &vfs_mnt);
        vmi_read_addr_va(vmiinfo->vmi, vfs_mnt, 0, &mnt_dentry);
        char pwdvfsdentry[255] = "";
        get_dentry_name(vmiinfo, pwdvfsdentry, mnt_dentry);
        ///printf("pwdvfs:%s ",pwdvfsdentry);

        char pwddentrystr[255] = "";
        get_dentry_name(vmiinfo, pwddentrystr, fspwd_dentry);
        int n = 3;
        while (n > 0 && pwddentrystr[0] != '/')
        {
            //n = n - 1;
            char pwddentry[255] = "";
            vmi_read_addr_va(vmiinfo->vmi, fspwd_dentry + vmiinfo->vmoffset[parent_offset], 0, &fspwd_dentry);
            strcat(pwddentrystr, "/");
            get_dentry_name(vmiinfo, pwddentry, fspwd_dentry);
            if (pwddentry[0] == '/')
            {
                n = 0;
            }
            else
            {
                strcat(pwddentrystr, pwddentry);
            }
        }
        ///printf("pwd:%s \n",pwddentrystr);
    }
} ///end get_fs_info

void get_task_info(VmiInfo* vmiinfo, addr_t current_process, TaskNode *tmptn)
{
    vmi_read_32_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[pid_offset], 0, (uint32_t *)&pid);
    vmi_read_32_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[tgid_offset], 0, (uint32_t *)&tgid);

    vmi_read_addr_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[task_real_parent_offset], 0, &task_real_parent);
    vmi_read_32_va(vmiinfo->vmi, task_real_parent + vmiinfo->vmoffset[pid_offset], 0, (uint32_t *)&real_parent_pid);
    vmi_read_addr_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[task_parent_offset], 0, &task_parent);
    vmi_read_32_va(vmiinfo->vmi, task_parent + vmiinfo->vmoffset[pid_offset], 0, (uint32_t *)&parent_pid);

    tmptn->tspid = pid;
    tmptn->tsparent = real_parent_pid;

    procname = vmi_read_str_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[name_offset], 0);
    strcpy(tmptn->tsname, procname);

    /*
    if (!procname)
    {
        printf("Failed to find procname\n");
        goto error_exit;
    }
    */

    /* print out the process name */
    ///printf("\n[%5d] [%5d] %s (struct addr:%lx)\n   rparent:%d  parent:%d ", pid, tgid, procname, current_process, real_parent_pid, parent_pid);
    ///printf("%d",pid);

    /*only show mm_struct info not add in struct*/
    get_mm_struct(vmiinfo, current_process, tmptn);

    vmi_read_addr_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[group_leader_offset], 0, &group_leader);
    vmi_read_32_va(vmiinfo->vmi, group_leader + vmiinfo->vmoffset[pid_offset], 0, (uint32_t *)&group_leader_pid);
    tmptn->tsgroupleader = group_leader_pid;
    ///printf("groupleader:%d ",group_leader_pid);

    /* show fs information */
    ///printf("fb");
    vmi_read_addr_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[fs_offset], 0, &fs);
    get_fs_info(vmiinfo, fs);
    ///printf("fa");

    /* show start time */
    uint64_t starttime = 0;
    vmi_read_64_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[start_time_offset], 0, (uint64_t *)&starttime);
    tmptn->tsstart_time = starttime;
    uint64_t realstarttime = 0;
    vmi_read_64_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[real_start_time_offset], 0, &realstarttime);
    tmptn->tsstart_time = starttime;
    tmptn->tsrealstart_time = realstarttime;
    ///printf("starttime:%lu  realstarttime:%lu ",starttime,realstarttime);

    /* show mm fault and swap info */
    unsigned long minflt = 0;
    vmi_read_64_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[min_flt_offset], 0, &minflt);
    unsigned long majflt = 0;
    vmi_read_64_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[maj_flt_offset], 0, &majflt);
    tmptn->tsminflt = minflt;
    tmptn->tsinc_minflt = minflt;
    tmptn->tsmajflt = majflt;
    tmptn->tsinc_majflt = majflt;
    ///printf("minflt:%lu majflt:%lu \n", minflt, majflt);

    /* show task prio */
    // first num same,the last num equal 0
    int prio, static_prio, normal_prio;
    unsigned int rt_priority;
    vmi_read_32_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[prio_offset], 0, &prio);
    vmi_read_32_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[static_prio_offset], 0, &static_prio);
    vmi_read_32_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[normal_prio_offset], 0, &normal_prio);
    vmi_read_32_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[rt_priority_offset], 0, &rt_priority);
    tmptn->tsprio = prio;
    ///printf("prio:%d static_prio:%d normal_prio:%d rt_priority:%u \n", prio, static_prio, normal_prio, rt_priority);

    /* show utime,stime,gtime */
    unsigned long utime = 0;
    unsigned long stime = 0;
    unsigned long utimescaled = 0;
    unsigned long stimescaled = 0;
    unsigned long gtime = 0;
    vmi_read_64_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[utime_offset], 0, &utime);
    vmi_read_64_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[stime_offset], 0, &stime);
    vmi_read_64_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[stime_offset] + 8, 0, &utimescaled);
    vmi_read_64_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[stime_offset] + 16, 0, &stimescaled);
    vmi_read_64_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[gtime_offset], 0, &gtime);
    tmptn->tsutime = utime;
    tmptn->tsstime = stime;
    tmptn->tsinc_utime = utime;
    tmptn->tsinc_stime = stime;
    ///printf("utime:%lu stime:%lu utimescaled:%lu stimescaled:%lu gtime:%lu\n", utime, stime, utimescaled, stimescaled, gtime);

    /*get task_io_accounting info*/
    uint64_t rchar = 0;
    uint64_t wchar = 0;
    uint64_t syscr = 0;
    uint64_t syscw = 0;
    uint64_t read_bytes = 0;
    uint64_t write_bytes = 0;
    uint64_t cancelled_write_bytes = 0;
    vmi_read_64_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[ioac_offset], 0, &rchar);
    vmi_read_64_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[ioac_offset] + 8, 0, &wchar);
    vmi_read_64_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[ioac_offset] + 16, 0, &syscr);
    vmi_read_64_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[ioac_offset] + 24, 0, &syscw);
    vmi_read_64_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[ioac_offset] + 32, 0, &read_bytes);
    vmi_read_64_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[ioac_offset] + 40, 0, &write_bytes);
    vmi_read_64_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[ioac_offset] + 48, 0, &cancelled_write_bytes);
    tmptn->tsrchar = rchar;
    tmptn->tswchar = wchar;
    tmptn->tssyscr = syscr;
    tmptn->tssyscw = syscw;
    tmptn->tsread_bytes = read_bytes;
    tmptn->tswrite_bytes = write_bytes;
    tmptn->tscancelled_write_bytes = cancelled_write_bytes;
    ///printf("rchar:%lu wchar:%lu syscr:%lu syscw:%lu read_bytes:%lu write_bytes:%lu cancelled_write_bytes:%lu\n", rchar, wchar, syscr, syscw, read_bytes, write_bytes, cancelled_write_bytes);

    vmi_read_addr_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[files_offset], 0, &files);
    if (VMI_FAILURE == vmi_read_addr_va(vmiinfo->vmi, files + vmiinfo->vmoffset[fdt_offset], 0, &fdt))
    {
        printf("%d %s Failed to read its files!!!!!\n", pid, procname);
    }
    vmi_read_addr_va(vmiinfo->vmi, files + 40, 0, &fdtab);
    vmi_read_addr_va(vmiinfo->vmi, files + 160, 0, &fd_array);
    vmi_read_32_va(vmiinfo->vmi, fdt + vmiinfo->vmoffset[maxfds_offset], 0, &max_fds);
    vmi_read_addr_va(vmiinfo->vmi, fdt + vmiinfo->vmoffset[fd_offset], 0, &fd);

    tmptn->tsfdnum = max_fds;

    // FdInfo tmpfdinfo;
    ///printf("tfb maxfiles:%d\n",max_fds);

    /* show files info about this task_struct */
    get_files_info(vmiinfo, fd, max_fds, tmptn);

    if(procname)
    {
        free(procname);
        procname = NULL;
    }
    ///printf("tfa\n");
} ///end get_task_info

addr_t get_psname_by_pid(VmiInfo* vmiinfo,vmi_pid_t pid,char* psname)
{
    addr_t current_process = 0;
    vmi_pid_t nowpid = 0;
    unsigned long tasks_offset = vmi_get_offset(vmiinfo->vmi, "linux_tasks");
    addr_t list_head = 0, next_list_entry = 0;
    next_list_entry = vmi_translate_ksym2v(vmiinfo->vmi, "init_task") + tasks_offset;
    list_head = next_list_entry;
    strcpy(psname,"none");
    do{
        current_process = next_list_entry - tasks_offset;
        vmi_read_32_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[pid_offset], 0, (uint32_t *)&nowpid);
        if(nowpid == pid){
            char* psnames = vmi_read_str_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[name_offset], 0);
            strcpy(psname,psnames);
            break;
        }
        vmi_read_addr_va(vmiinfo->vmi, next_list_entry, 0, &next_list_entry);
        
    }
    while(next_list_entry != list_head);
    return current_process;

}

void get_psname_by_addr(VmiInfo* vmiinfo,char* psname,addr_t current_process)
{ 
    char* psnames = vmi_read_str_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[name_offset], 0);
    if(strcmp(psname,psnames)!=0){
        printf("Modify name %s to %s\n",psname,psnames);
        strcpy(psname,psnames);
        record_ps_list(vmiinfo);
    }
    free(psnames);

}
/*
void setoffset(VmiInfo *vmiinfo, char *key, unsigned long value)
{
    int i = 0;
    for (; i < vmiinfo->offset_len; i++)
    {
        if (0 == strcmp((vmiinfo->vmoffset[i]).key, key))
        {
            vmiinfo->vmoffset[i].value = value;
            return;
        }
    }
    printf("no offset %s", key);
}

int getoffset(VmiInfo *vmiinfo, char *key)
{
    int i = 0;
    for (; i < vmiinfo->offset_len; i++)
    {
        if (0 == strcmp((vmiinfo->vmoffset[i]).key, key))
        {
            return vmiinfo->vmoffset[i].value;
        }
    }
    printf("no offset %s", key);
    return -1;
}
*/

void initPs(VmiInfo* vmiinfo,addr_t list_head){
    addr_t next_list_e = list_head;
    addr_t current_process;
    int psnum = 0;
    MyList * initps_list= createMySearchList(compare2ps);
    vmi_pause_vm(vmiinfo->vmi);
    do
    {
        psnum++;
        current_process = next_list_e - vmiinfo->vmoffset[task_offset];

        PsNode *oneps = malloc(sizeof(PsNode));
        oneps->addr = current_process;
        vmi_read_32_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[pid_offset], 0, &(oneps->pid));
        procname = vmi_read_str_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[name_offset], 0);
        strcpy(oneps->name, procname);
        addr_t mm_structaddr = 1;
        vmi_read_addr_va(vmiinfo->vmi, current_process + vmiinfo->vmoffset[mm_offset], 0, &mm_structaddr);
        vmi_read_64_va(vmiinfo->vmi, mm_structaddr + vmiinfo->vmoffset[pgd_offset], 0, &(oneps->pgd));
        oneps->pgd = oneps->pgd&0x00000000ffffffff;
        ///printf("%s(%d) addr:%lx pgd:%lx\n",oneps->name,oneps->pid,oneps->addr,oneps->pgd);
        myListInsertDataAtLast(initps_list, oneps);

        /* follow the next pointer */
        status_t status = vmi_read_addr_va(vmiinfo->vmi, next_list_e, 0, &next_list_e);
        if (status == VMI_FAILURE)
        {
            printf("Failed to read next pointer in loop at \n");
            break;
        }

    } while (next_list_e != list_head);
    vmi_resume_vm(vmiinfo->vmi);
    vmiinfo->pslist = initps_list;
    vmiinfo->ps_num = psnum;
    record_ps_list(vmiinfo);
    printf("init ps ok num:%d!!\n",psnum);
}

PsNode* get_ps_fron_pgd(VmiInfo* vmiinfo,addr_t nowpgd){
    
    MyNode * p = vmiinfo->pslist->first;
    int i = 0;
    for (; i < vmiinfo->pslist->count; i++)
    {
        PsNode* aa = p->data;
        //printf("pgd:%lx find:%lx\n",aa->pgd,nowpgd);
        if(aa->pgd == nowpgd){
            //printf("find ps!\n");
            return (PsNode*)p->data;
        }
        p = p->next;
    }
    //printf("no find ps!\n");
    return NULL;
}

PsNode* get_ps_fron_pid(VmiInfo* vmiinfo,vmi_pid_t pid){   
    MyNode * p = vmiinfo->pslist->first;
    int i = 0;
    for (; i < vmiinfo->pslist->count; i++)
    {
        PsNode* aa = p->data;
        //printf("pgd:%lx find:%lx\n",aa->pgd,nowpgd);
        if(aa->pid == pid){
            //printf("find ps!\n");
            return (PsNode*)p->data;
        }
        p = p->next;
    }
    //printf("no find ps!\n");
    return NULL;
}

void delete_one_ps(VmiInfo* vmiinfo,vmi_pid_t pid){
    MyNode * p = vmiinfo->pslist->first;
    int deletere = -1;
    int re = 0;
    //printf("11\n");

    while (p)
    {
        PsNode *aa = p->data;
        if (aa->pid==pid)
        {
            deletere=re;
            printf("find %d\n",pid);
            (vmiinfo->ps_num)--;
            break;
        }
        re++;
        p = p->next;
    }
    //printf("22\n");
    if (deletere != -1)
    {
        printf("deltet ps total:%d\n",vmiinfo->ps_num);
        myListRemoveDataAt(vmiinfo->pslist, deletere);
        record_ps_list(vmiinfo);
    }
   // printf("33\n");
}

int compare2ps(void* a, void* b){
    PsNode* aa = a;
    PsNode* bb = b;
    if(strcmp(aa->name,bb->name)==0&&aa->pid==bb->pid&&aa->pgd==bb->pgd)
        return 1;
    else
        return 0;
    return 0;
}

//insert ps
void add_pslist(VmiInfo* vmiinfo,PsNode* psnode)
{
    MyNode *p = vmiinfo->pslist->last;
    PsNode* aa = p->data;
    if (aa->pid < psnode->pid)
    {
        myListInsertDataAtLast(vmiinfo->pslist, psnode);
        return;
    }

    MyNode * node = (MyNode *) malloc(sizeof(MyNode));
    node->data = psnode;
    node->next = NULL;

    p = vmiinfo->pslist->first;
    while(p->next){
        aa = p->next->data;
        if (aa->pid == psnode->pid)
        {
            strcpy(aa->name,psnode->name);
            aa->pgd = psnode->pgd;
            
            return;
            
        }
        if (aa->pid > psnode->pid)
        {
            break;
        }
        p = p->next;
    }
    node->next = p->next;
    p->next = node;
    (vmiinfo->pslist->count)++;
}

void record_ps_list(VmiInfo* vmiinfo){
    MyNode *p = vmiinfo->pslist->first;
    PsNode *aa;
    char pslogfile[100]="temple/";
    strcat(pslogfile,vmiinfo->vmname);
    strcat(pslogfile,".pslist");
    FILE *pf = fopen(pslogfile,"w");
    while (p)
    {
        aa = p->data;
        fprintf(pf,"%d %s %lx %lx \n", aa->pid,aa->name,aa->addr,aa->pgd);
        p = p->next;
    }
    fclose(pf);
}
void clear_ps_file(VmiInfo* vmiinfo){
    MyNode *p = vmiinfo->pslist->first;
    PsNode *aa;
    char pslogfile[100]="temple/";
    strcat(pslogfile,vmiinfo->vmname);
    strcat(pslogfile,".pslist");
    FILE *pf = fopen(pslogfile,"w");
    fclose(pf);
}