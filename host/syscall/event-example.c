#include <signal.h>
#include <libvmi/libvmi.h>
#include <libvmi/events.h>
#include <stdio.h>

#include "event-example.h"

static uint8_t trap = 0xCC;
static uint8_t backup_byte;
static uint8_t socket_backup_byte;
//uint64_t sys_call_table_addr = 18446744071589322224u;
addr_t sys_call_table_addr = 0xffffffff81216840;

addr_t sys_call_socket_addr = 0xffffffff8170f7c0;
//addr_t sys_call_socket_addr = 0xffffffff81a001c0;//sys_call_enter

int syscallnum[NUMBER_OF_SYSCALLS]= {0};

reg_t rdi, rax,cr3;
reg_t regrip = 0;
static int i;
reg_t sysenter_ip = 0;
addr_t phys_sysenter_ip = 0;

/* Signal handler */
static struct sigaction act;
static int interrupted = 0;
static void close_handler(int sig)
{
    interrupted = sig;
}
int k = 0;
int ec = 0;
int sys_num = -1;
event_response_t singlestep_cb(vmi_instance_t vmi, vmi_event_t *event)
{
    ///printf("enter one ");

    vmi_write_8_va(vmi, syscalls[sys_num].addr, 0, &trap);
//    if(sys_num == 59)
//    {
//        vmi_write_8_va(vmi, sys_call_table_addr, 0, &trap);
//    }
//    if(sys_num == 41)
//    {
//        vmi_write_8_va(vmi, sys_call_socket_addr, 0, &trap);
//    }
    printf("%d one step %d\n",rax,k);
    ++k;
    //vmi_set_vcpureg(vmi,trap,SYSENTER_EIP,0);
    //vmi_set_vcpureg(vmi,trap,RIP,0);
    return VMI_EVENT_RESPONSE_TOGGLE_SINGLESTEP;
}

event_response_t trap_cb(vmi_instance_t vmi, vmi_event_t *event)
{


    vmi_get_vcpureg(vmi, &rax, RAX, event->vcpu_id);
    // vmi_get_vcpureg(vmi, &rdi, RDI, event->vcpu_id);
    vmi_get_vcpureg(vmi, &cr3, CR3, event->vcpu_id);
    vmi_pid_t pid = vmi_dtb_to_pid(vmi, cr3);


    printf("%d  syscall#=%u ec:%d \n", pid,(unsigned int)rax,ec);


//reg_t rbx, rcx,rdx;
    //vmi_get_vcpureg(vmi, &rbx, TR_SEL, event->vcpu_id);
    // vmi_get_vcpureg(vmi, &rcx, LDTR_SEL, event->vcpu_id);
    // vmi_get_vcpureg(vmi, &rdx, RIP, event->vcpu_id);
    //printf("%d Syscall happened: RAX(syscall#)=%u RDI(1st argument)=%u\n", pid,(unsigned int)rax, (unsigned int)rdi);
    //printf("rbx:%x rcx:%x rdx:%x cr3:%x\n", (unsigned int)rbx, (unsigned int)rcx,(unsigned int)rdx,(unsigned int)cr3);
    //printf("Received a trap event for syscall %s!\n", syscalls[i].name);
    sys_num = rax;
    syscallnum[sys_num]++;
    vmi_write_8_va(vmi, syscalls[sys_num].addr, 0, &(syscalls[sys_num].pre));
//    if(rax == 59)
//    {
//        vmi_write_8_va(vmi, sys_call_table_addr, 0, &backup_byte);
//    }
//    if(rax == 41)
//    {
//        vmi_write_8_va(vmi, sys_call_socket_addr, 0, &socket_backup_byte);
//    }
    // vmi_set_vcpureg(vmi,backup_byte,SYSENTER_EIP,0);
    //vmi_set_vcpureg(vmi,regrip,RIP,0);

    event->interrupt_event.reinject = 0;
    ///printf("return\n");
//    if(ec!=k)
//    {
//        ec = k;
//        printf("!!\n");
//        return 0;
//    }
    ++ec;

    return VMI_EVENT_RESPONSE_TOGGLE_SINGLESTEP;
}

int main(int argc, char **argv)
{

//    if (argc != 3) {
//        printf("Usage: %s <domain name> <syscall_name>\n");
//        return 1;
//    }

    status_t status;
    const char *name = argv[1];
    //const char *syscall = argv[2];

    /* Signal handler to catch CTRL+C, etc.. */
    act.sa_handler = close_handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGHUP,  &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT,  &act, NULL);
    sigaction(SIGALRM, &act, NULL);

    vmi_instance_t vmi;
    // Initialize the libvmi library.
    if (VMI_FAILURE == vmi_init_complete(&vmi, name, VMI_INIT_DOMAINNAME | VMI_INIT_EVENTS, NULL, VMI_CONFIG_GLOBAL_FILE_ENTRY, NULL, NULL))
    {
        //if (VMI_FAILURE == vmi_init(&vmi, VMI_XEN,name, VMI_INIT_DOMAINNAME | VMI_INIT_EVENTS, NULL, NULL)) {
        printf("Failed to init LibVMI library.\n");
        return;
    }
    ////////////////EIP addr///////////////
//	vmi_get_vcpureg(vmi, &sysenter_ip, SYSENTER_EIP, 0);
//	vmi_get_vcpureg(vmi, &backup_byte, SYSENTER_EIP, 0);
//	printf("vcpu 0 MSR_SYSENTER_IP == %llx\n", (unsigned long long)sysenter_ip);
//    phys_sysenter_ip= vmi_translate_kv2p(vmi, sysenter_ip);
//    printf("Physical SYSENTER_IP == %llx\n", (unsigned long long)phys_sysenter_ip);
//
//    vmi_get_vcpureg(vmi, &regrip, RIP, 0);
//	printf("vcpu 0 RIP == %llx\n", (unsigned long long)regrip);


    ////////////////////END//////////////////////////////////////////

    vmi_pause_vm(vmi);

    ////////////////////set eip/////////////////////////////////
    //vmi_set_vcpureg(vmi,trap,SYSENTER_EIP,0);
    //vmi_set_vcpureg(vmi,trap,RIP,0);

    /////////////////////END////////////////////////////

//    for (i=0; i < NUMBER_OF_SYSCALLS; i++) {
//        //printf("%s at 0x%lx\n", syscalls[i].name, syscalls[i].addr);
//
//        if(!strcmp(syscalls[i].name, syscall)) {
//            printf("Syscall %s is located at VA 0x%lx\n", syscall, syscalls[i].addr);
//			vmi_read_8_va(vmi, syscalls[i].addr, 0, &backup_byte);
//			vmi_write_8_va(vmi, syscalls[i].addr, 0, &trap);
//            break;
//		}
//	}

    uint64_t beforemodify;
    int i;
    for(i = 0; i < NUMBER_OF_SYSCALLS; i++)
    {
        if(syscalls[i].addr!=0)
        {
            // vmi_read_64_va(vmi, sys_call_table_addr, 0, &beforemodify);
            // printf("sys_call_table_entry_addr == %llx\n", sys_call_table_addr);
            // printf("before modify sys_call_table_entry_addr == %llx\n", beforemodify);

            vmi_read_8_va(vmi, syscalls[i].addr, 0, &backup_byte);
            vmi_write_8_va(vmi, syscalls[i].addr, 0, &trap);
            syscalls[i].pre = backup_byte;
        }
        else
        {
            printf("syscall:%d no change\n",i);
            syscallnum[i] = -1;
        }

        // reg_t cr0,cr00;
        // vmi_get_vcpureg(vmi, &cr0, CR0, 0);
        // cr00 = cr0&0xFFFEFFFF;
        // printf("CR0:%x\n",cr00);
        //vmi_set_vcpureg(vmi,cr00,CR0,0);
        //vmi_read_8_va(vmi, sys_call_socket_addr, 0, &socket_backup_byte);
        //vmi_write_8_va(vmi, sys_call_socket_addr, 0, &trap);
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
    scanf("%d",&a);

    vmi_resume_vm(vmi);

    uint64_t aftermodify;
    vmi_read_64_va(vmi, sys_call_table_addr, 0, &aftermodify);
    vmi_get_vcpureg(vmi, &sysenter_ip, SYSENTER_EIP, 0);
    printf("vcpu 0 MSR_SYSENTER_IP == %llx\n", (unsigned long long)sysenter_ip);
    printf("sys_call_table_entry_addr == %llx\n", backup_byte);
    printf("afte modify sys_call_table_entry_addr == %llx\n", aftermodify);

    while(!interrupted)
    {
        status = vmi_events_listen(vmi,500);
        if (status != VMI_SUCCESS)
        {
            printf("Error waiting for events, quitting...\n");
            interrupted = -1;
        }
    }
    FILE *f3;
    f3 = fopen("syscallres.txt","w");

    // TODO 4: remove traps and close LibVMI
    fprintf(f3,"total : %d\n",k);
    printf("total k:%d ec:%d \n",k,ec);
    for(i = 0; i < NUMBER_OF_SYSCALLS; i++)
    {
        printf("%s :%d\n",syscalls[i].name,syscallnum[i]);
        fprintf(f3,"%s :%d\n",syscalls[i].name,syscallnum[i]);
        if(syscalls[i].addr!=0)
        {
            vmi_write_8_va(vmi, syscalls[i].addr, 0, &(syscalls[i].pre));
        }

    }
    // vmi_write_8_va(vmi, sys_call_table_addr, 0, &backup_byte);
    // vmi_write_8_va(vmi, sys_call_socket_addr, 0, &socket_backup_byte);
    // vmi_set_vcpureg(vmi,backup_byte,SYSENTER_EIP,0);
    //vmi_set_vcpureg(vmi,regrip,RIP,0);

    vmi_destroy(vmi);
    // vmi_set_vcpureg(vmi,cr0,CR0,0);
    printf("over\n");
    return 0;
}
