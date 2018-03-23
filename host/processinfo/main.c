/**
* Copyright(c) 2017 dyf
* the main process to get process info
*/
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdio.h>
#include <inttypes.h>

#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <libvmi/libvmi.h>
#include <linux/kernel.h>
#include <linux/sched.h>

#include <linux/version.h>

#include "myqueue.h"
#include "mysyscall.h"
#include "mypsinfo.h"
#include "vminit.h"
#include "mydbsql.h"
#include "myList.h"
#include "acl.h"
#include "mymodinfo.h"

int main (int argc, char **argv)
{
    extern int vm_num;
    extern VmiInfo* globalvm[];
    vmi_instance_t vmi;
    addr_t list_head = 0, next_list_entry = 0;
    unsigned long tasks_offset = 0;
    addr_t current_process = 0;
    status_t status;

    /* this is the VM or file that we are looking at */
    if (argc != 3)
    {
        printf("Usage: %s <vmname>\n", argv[0]);
        return 1;
    } // if

    char *name = argv[1];
    int table;
    sscanf(argv[2],"%d",&table);
    

    /* initialize the libvmi library */
    if (VMI_FAILURE ==
            vmi_init_complete(&vmi, name, VMI_INIT_DOMAINNAME , NULL,
                              VMI_CONFIG_GLOBAL_FILE_ENTRY, NULL, NULL))
    {
        printf("Failed to init LibVMI library.\n");
        return 1;
    }
    else{
         printf("%s success to init LibVMI library.\n",name);
    }

    /* init the offset values */
    if (VMI_OS_LINUX == vmi_get_ostype(vmi))
    {
        tasks_offset = vmi_get_offset(vmi, "linux_tasks");
        //unsigned long name_offset = vmi_get_offset(vmi, "linux_name");
        //unsigned long pid_offset = vmi_get_offset(vmi, "linux_pid");
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
    vmi_resume_vm(vmi);

    
    VmiInfo* vmivm = (VmiInfo*)malloc(sizeof(VmiInfo));
    vmivm->vmi = vmi;
    strcpy(vmivm->vmname,name);
    // Initialize the vm version.
    clear_filelog();
    if(0 == read_version_conf(vmivm))
    {
        printf("Failed to init vm version.\n");
        return 1;
    }
    vmivm->vmid = vmi_get_vmid(vmi);
    // Initialize the vm offset.
    if(0 == read_offset_conf(vmivm))
    {
        printf("Failed to init vm offset.\n");
        return 1;
    }

     // Initialize the vm syscall.
    if(0 == read_syscall_conf(vmivm))
    {
        printf("Failed to init vm syscall.\n");
        return 1;
    }

    next_list_entry = list_head;
    // Initialize the ps list.
    initPs(vmivm, list_head);

    //init the module list
    initModule(vmivm);

    int fdpipe[2];
    char buf[256];
    pid_t fpid;
    MYSQL mysql;

    init_db(&mysql);
    if(conn_db(&mysql, "localhost", "root", "309911", "vmidata") != 0)
    {
        return;
    }

    if( pipe(fdpipe)<0 )
    {
        perror("failed to pipe");
        exit(1);
    }
    if( (fpid = fork())<0 )
    {
        perror("failed to fork");
        exit(1);
    }
    else if(fpid > 0)
    {
        ///printf("%d father start!\n",fpid);
       
        ///init the ACL list of ps
        MyList * acl_list= createMySearchList(compare2aclps);
        getACLList(vmivm,acl_list);
        ///myListOutput(acl_list, outputACL);

        

        /* walk the task list */
        int n = 1000;
        int readn;
        int flags = fcntl(fdpipe[0], F_GETFL);//get pipe flags
        fcntl(fdpipe[0],F_SETFL,flags | O_NONBLOCK);//modify flags
        psyscall getsyscall;
        LinkQueue* pre_queue = malloc(sizeof(LinkQueue*));
        initQueue(pre_queue);
        LinkQueue* queue = malloc(sizeof(LinkQueue*));

        /**the time enter traveral
         * 1 only insert change 
         * 2 insert the first all process and change
        */
        int frenum = 1;

        while(n>0)
        {
            int psnum = 0;
            int module_change = 0;

            //task queue
            initQueue(queue);
            
            ///vmi_pause_vm(vmi);

            ///while(false)///test fork
            
         
            do
            {
                psnum++;
                ///printf("\n");

                TaskNode *tasknodetmp = (TaskNode*)calloc(1,sizeof(TaskNode));
                //printf("!!first!!minflt:%d ",tasknodetmp.minflt);
                current_process = next_list_entry - tasks_offset;
                ///printf(" %d-",psnum);
                get_task_info(vmivm,current_process,tasknodetmp);
                tasknodetmp->tslayer = -1;
                


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

                //tasknodetmp.tsfdinfo = tmpfdinfo;

                /* modify the state,if the ps in the ACL or not
                0 have not detect
                1 normal
                2 anomaly
                3 ACL*/
                tasknodetmp->state = 0;
                if(findACLps(acl_list,tasknodetmp->tsname,tasknodetmp->tspid) > -1){
                    tasknodetmp->state = 3;
                    ///printf("ACL: ");
                }
                ///printf("%d:%s %d\n",psnum,tasknodetmp->tsname,tasknodetmp->tspid);

                /*push the ps into the list*/
                pushQueue(queue,tasknodetmp);

                /* follow the next pointer */
                status = vmi_read_addr_va(vmi, next_list_entry, 0, &next_list_entry);
                if (status == VMI_FAILURE)
                {
                    printf("Failed to read next pointer in loop at %"PRIx64"\n", next_list_entry);
                    goto error_exit;
                }

            }
            while(next_list_entry != list_head);

            MyList* pslist_real = createMySearchList(compare2ps_nameid);
            read_pslist_from_file(vmivm,pslist_real);
            ///vmi_resume_vm(vmi);
            ///printf("1:ok \n");

            int sysclassnum = 10;
            int pssystotal[psnum+1][sysclassnum+1];
            int i = 0;
            TaskNode* q = queue->front->next;

            for(; i < psnum; i++)
            {
                pssystotal[i][0] = q->tspid;

                int j = 1;
                for(; j<sysclassnum+1; j++)
                {
                    pssystotal[i][j] = 0;
                }
                q = q->next;
            }
            int j = 0;
            for(; j<sysclassnum+1; j++)
            {
                pssystotal[psnum][j] = 0;
            }

            ///test fork
            n--;
            //printf("This is farther, write hello to pipe\n");
            //write(fd[1], "hello\n", 25);
            

            int getsysnum = 0;
            do
            {
                readn = read(fdpipe[0], &getsyscall, sizeof(psyscall));
                if(getsyscall.sysnum == 176){ 
                    module_change++;
                }
                if(getsyscall.sysnum == 313){ 
                    module_change--;
                }
                i = 0;
                for(; i < psnum; i++)
                {
                    if(pssystotal[i][0] == getsyscall.pid)
                    {
                        pssystotal[i][vmivm->syscallall[getsyscall.sysnum].classify+1]++;
                        pssystotal[psnum][vmivm->syscallall[getsyscall.sysnum].classify+1]++;
                        i = psnum;
                        break;
                    }   
                }

                ++getsysnum;
               /// printf("%d father get %d do syscall %d\n",readn, getsyscall.pid,getsyscall.sysnum);

            }
            while(readn>0);
            /**detect hide module*/
            module_change = detect_hide_module(vmivm,&mysql,module_change);

            /**detect hide os*/
            detect_hide_ps(vmivm,&mysql,queue,pslist_real);
            

            ///printf("2:ok \n");
            combineSyscallAndPs(queue,pssystotal,psnum);

            setParentLayer(queue);

            traversal(vmivm,&mysql,queue,pre_queue,frenum,table);
            if(frenum > 0){
                frenum--;
            }
            comm_db(&mysql);


            i = psnum;/// i = 0
            for(; i < psnum+1; i++)
            {
                int j = 1;
                printf("%d syscall:",pssystotal[i][0]);
                for(; j<sysclassnum+1; j++)
                {
                    printf(" %d(%d)",pssystotal[i][j],j-1);
                    pssystotal[i][j] = 0;
                }
                printf("\n");
            }


            freeMyList(pslist_real);

            time_t now;
            struct tm *timenow;
            time(&now);
            //timenow = localtime(&now);
            printf("Local   time   is   %ld\n",now);


            printf("father get %d syscall sleep 1\n\n",getsysnum);
            sleep(1);
            //getsysnum = 0;
        }
        disconn_db(&mysql);
        printf("father exit \n");

        //vmi_destroy(vmi);
        //exit(0);
    }
    else
    {
        ///printf("%d child start!!\n",fpid);
       
        pid_t ppid = getppid();
        extern int pipenum;
        pipenum = fdpipe[1];

        /* Signal handler to catch CTRL+C, etc.. */

        act.sa_handler = close_handler;
        act.sa_flags = 0;
        sigemptyset(&act.sa_mask);
        sigaction(SIGHUP,  &act, NULL);
        sigaction(SIGTERM, &act, NULL);
        sigaction(SIGINT,  &act, NULL);
        sigaction(SIGALRM, &act, NULL);
        uint64_t beforemodify;
        
        static uint8_t trap = 0xCC;
        reg_t sysenter_ip = 0;
        addr_t phys_sysenter_ip = 0;
        addr_t sys_call_table_addr = 0xffffffff81a001c0;
        ///vmi_instance_t vmi;
        // Initialize the libvmi library.
 
        if (VMI_FAILURE == vmi_init_complete(&vmi, name, VMI_INIT_DOMAINNAME | VMI_INIT_EVENTS , NULL,
                                             VMI_CONFIG_GLOBAL_FILE_ENTRY, NULL, NULL))
        {
            printf("Failed to init LibVMI library.\n");
            return 1;
        }
        vmivm->vmi = vmi;

        vmi_pause_vm(vmi);

        uint64_t backup_byte = 0;
        uint64_t code = 0;
        extern int syscallnum[];
        printf("start detcet syscall hook!\n");
        FILE *pftest = fopen("log/test.log","a");
        time_t timep;
        time(&timep);
        fprintf(pftest,"%s!!\n",ctime(&timep));
        int i;
        for(i = 0; i < vmivm->syscall_len; i++)
        {
            if(vmivm->syscallall[i].addr != 0)
            {
                //printf("syscall num %d",i);
                vmi_read_8_va(vmi, vmivm->syscallall[i].addr, 0, &(vmivm->syscallall[i].pre));
                vmi_read_64_va(vmi, sys_call_table_addr+i*8, 0, &backup_byte);
                vmi_read_64_va(vmi, vmivm->syscallall[i].addr, 0, &code);
                if(vmivm->syscallall[i].sign == 1){
                    vmi_write_8_va(vmi, vmivm->syscallall[i].addr, 0, &trap);
                }
                else{
                    printf("syscall:%d no change \n",i);
                }
                ///printf("!!%d addr:%lx backup_byte:%lx right:%x code:%lx\n",i,vmivm->syscallall[i].addr,backup_byte,vmivm->syscallall[i].pre,code);
                if(vmivm->syscallall[i].addr != backup_byte){
                    fprintf(pftest,"%d %s read in right addr:%lx was hooked to %lx!!\n",i,vmivm->syscallall[i].name,vmivm->syscallall[i].reallyaddr,backup_byte);
                    printf("%d %s right addr:%lx was hooked to %lx!!\n",i,vmivm->syscallall[i].name,vmivm->syscallall[i].addr,backup_byte);
                    vmi_write_64_va(vmi, sys_call_table_addr+i*8, 0, &(vmivm->syscallall[i].addr));
                    printf("recover %s into right addr!\n",vmivm->syscallall[i].name);
                    find_syscall_hook(vmivm,&mysql,i,backup_byte);
                   /// printf("\n%d addr:%s read in right addr:%lx was hooked!!\n",i,vmivm->syscallall[i].name,vmivm->syscallall[i].reallyaddr);
                }
            }
            else
            {
                printf("syscall:%d no addr\n",i);
                syscallnum[i] = -1;
            }

        }
        fclose(pftest);
        // printf("CR0 set over\n");
        
        vmi_event_t trap_event, singlestep_event;
        SETUP_INTERRUPT_EVENT(&trap_event, 0, trap_cb);
        SETUP_SINGLESTEP_EVENT(&singlestep_event, 1, singlestep_cb, 0);
        vmi_register_event(vmi, &trap_event);
        vmi_register_event(vmi, &singlestep_event);
        reg_t cr0;
//    vmi_get_vcpureg(vmi, &cr0, CR0, 0);
//    printf("cr0:%x",cr0);
       
        globalvm[vm_num] = vmivm;

        vmi_resume_vm(vmi);

        uint64_t aftermodify;
        vmi_read_64_va(vmi, sys_call_table_addr+16, 0, &aftermodify);
        vmi_get_vcpureg(vmi, &sysenter_ip, SYSENTER_EIP, 0);
        ///printf("vcpu 0 MSR_SYSENTER_IP == %llx\n", (unsigned long long)sysenter_ip);
        ///printf("sys_call_table_entry_addr == %lx\n", backup_byte);
        ///printf("afte modify sys_call_table_entry_addr == %lx\n", aftermodify);
        int n1 = 5000;

        while(!interrupted&&ppid == getppid())
        {
            status = vmi_events_listen(vmi,500);
            if (status != VMI_SUCCESS)
            {
                printf("Error waiting for events, quitting...\n");
                interrupted = -1;
            }

        }
        FILE *pfover = fopen("log/cprint.log","a");
        for(i = 0; i < vmivm->syscall_len; i++)
        {
            fprintf(pfover,"%3d %s :%d\n",i,vmivm->syscallall[i].name,syscallnum[i]);
            if(vmivm->syscallall[i].sign == 1)
            {
                ///printf("%d ok ",i );
                vmi_write_8_va(vmi, vmivm->syscallall[i].addr, 0, &(vmivm->syscallall[i].pre));   
            }

        }
        fclose(pfover);
        clear_ps_file(vmivm);
        //vmi_resume_vm(vmi);
        vmi_destroy(vmi);
        extern int trapnum;
        extern int singstepnum;
        printf("trap:%d , singstep:%d\n",trapnum,singstepnum);

        printf("child exit\n");
        exit(0);
    }

    printf("all exit before\n");
error_exit:
    printf("all exit after\n");
    /* resume the vm */
    ///vmi_resume_vm(vmi);

    /* cleanup any memory associated with the LibVMI instance */
    ///vmi_destroy(vmi);

    return 0;
}
