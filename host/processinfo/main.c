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

#include <libvmi/libvmi.h>
#include <linux/kernel.h>
#include <linux/sched.h>

#include <linux/version.h>

#include "myqueue.h"
#include "mysyscall.h"
#include "mypsinfo.h"
#include "mydbsql.h"










int main (int argc, char **argv)
{
    vmi_instance_t vmi;
    addr_t list_head = 0, next_list_entry = 0;
    unsigned long tasks_offset = 0;
    addr_t current_process = 0;


    /* this is the VM or file that we are looking at */
    if (argc != 2)
    {
        printf("Usage: %s <vmname>\n", argv[0]);
        return 1;
    } // if

    char *name = argv[1];

    /* initialize the libvmi library */
    if (VMI_FAILURE ==
            vmi_init_complete(&vmi, name, VMI_INIT_DOMAINNAME , NULL,
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
    vmi_resume_vm(vmi);



    next_list_entry = list_head;

    //task queue
    LinkQueue queue;
    initQueue(&queue);


    int fdpipe[2];
    char buf[256];
    pid_t fpid;


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
        printf("%d father start!\n",fpid);

        init_db();
        if (conn_db("localhost", "root", "309911", "vmidata") != 0)
        {
            return;
        }



        /* walk the task list */
        int n = 5;
        int readn;
        int flags = fcntl(fdpipe[0], F_GETFL);//ÏÈ»ñÈ¡Ô­ÏÈµÄflags
        fcntl(fdpipe[0],F_SETFL,flags | O_NONBLOCK);//ÉèÖ
        psyscall getsyscall;

        while(n>0)
        {
            int psnum = 0;

            ///while(false)///test fork
            do
            {
                psnum++;

                TaskNode *tasknodetmp = (TaskNode*)calloc(1,sizeof(TaskNode));
                //printf("!!first!!minflt:%d ",tasknodetmp.minflt);
                current_process = next_list_entry - tasks_offset;
                get_task_info(vmi,current_process,tasknodetmp);


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


                pushQueue(&queue,tasknodetmp);



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


            int sysclassnum = 10;
            int pssystotal[psnum+1][sysclassnum+1];
            int i = 0;
            TaskNode* q = queue.front->next;
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
            sleep(1);
            int getsysnum = 0;
            do
            {
                readn = read(fdpipe[0], &getsyscall, sizeof(psyscall));
                i = 0;
                for(; i < psnum; i++)
                {
                    if(pssystotal[i][0] == getsyscall.pid)
                    {
                        pssystotal[i][syscalls[getsyscall.sysnum].classify+1]++;
                        pssystotal[psnum][syscalls[getsyscall.sysnum].classify+1]++;
                        i = psnum;
                    }
                }
                ++getsysnum;
                ///printf("%d father get %d do syscall %d\n",readn, getsyscall.pid,getsyscall.sysnum);

            }
            while(readn>0);

            traversal(queue,pssystotal,psnum);
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

            printf("father get %d syscall sleep 1\n\n",getsysnum);
            //getsysnum = 0;
        }
        disconn_db();
        printf("father exit \n");

        //vmi_destroy(vmi);
        //exit(0);
    }
    else
    {
        printf("%d child start!!\n",fpid);
        pid_t ppid = getppid();
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
        int i;
        static uint8_t trap = 0xCC;
        static uint8_t backup_byte;
        reg_t sysenter_ip = 0;
        addr_t phys_sysenter_ip = 0;
        addr_t sys_call_table_addr = 0xffffffff81216840;
        vmi_instance_t vmi;
        // Initialize the libvmi library.
        if (VMI_FAILURE == vmi_init_complete(&vmi, name, VMI_INIT_DOMAINNAME | VMI_INIT_EVENTS , NULL,
                                             VMI_CONFIG_GLOBAL_FILE_ENTRY, NULL, NULL))
        {
            printf("Failed to init LibVMI library.\n");
            return 1;
        }


        vmi_pause_vm(vmi);

        for(i = 0; i < NUMBER_OF_SYSCALLS; i++)
        {
            if(syscalls[i].addr!=0)
            {
                //printf("syscall num %d",i);
                vmi_read_8_va(vmi, syscalls[i].addr, 0, &backup_byte);
                vmi_write_8_va(vmi, syscalls[i].addr, 0, &trap);
                syscalls[i].pre = backup_byte;
            }
            else
            {
                printf("syscall:%d no change\n",i);
                syscallnum[i] = -1;
            }

        }
        // printf("CR0 set over\n");
        vmi_event_t trap_event, singlestep_event;
        SETUP_INTERRUPT_EVENT(&trap_event, 0, trap_cb);
        SETUP_SINGLESTEP_EVENT(&singlestep_event, 1, singlestep_cb, 0);
        vmi_register_event(vmi, &trap_event);
        vmi_register_event(vmi, &singlestep_event);
        reg_t cr0;
//    vmi_get_vcpureg(vmi, &cr0, CR0, 0);
//    printf("cr0:%x",cr0);
        int a;
        // scanf("%d",&a);

        vmi_resume_vm(vmi);

        uint64_t aftermodify;
        vmi_read_64_va(vmi, sys_call_table_addr, 0, &aftermodify);
        vmi_get_vcpureg(vmi, &sysenter_ip, SYSENTER_EIP, 0);
        printf("vcpu 0 MSR_SYSENTER_IP == %llx\n", (unsigned long long)sysenter_ip);
        printf("sys_call_table_entry_addr == %x\n", backup_byte);
        printf("afte modify sys_call_table_entry_addr == %lx\n", aftermodify);
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

        for(i = 0; i < NUMBER_OF_SYSCALLS; i++)
        {
            ///printf("%s :%d\n",syscalls[i].name,syscallnum[i]);
            //fprintf(f3,"%s :%d\n",syscalls[i].name,syscallnum[i]);
            if(syscalls[i].addr!=0)
            {
                ///printf("%d ok ",i );
                vmi_write_8_va(vmi, syscalls[i].addr, 0, &(syscalls[i].pre));
            }

        }
        //vmi_resume_vm(vmi);
        vmi_destroy(vmi);
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
