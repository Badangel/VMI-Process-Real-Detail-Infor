#!/usr/bin/python
import pyvmi
import sys
import time
import socket
from state.warning_log import *

def numtoip(num):
    num=socket.htonl(num)
    ch2 = lambda x: '.'.join([str(x/(256**i)%256) for i in range(3,-1,-1)])
    return ch2(num)

def numtoipv6(num):
    num=socket.htonl(num)
    ch2 = lambda x: ':'.join([str(hex(x/(65536**i)%65536))[2:-1] for i in range(7,-1,-1)])
    return ch2(num)

def get_os_params(vmi):
    ostype = vmi.get_ostype()

    if ostype.lower() == "windows":
        return get_windows_params(vmi)
    elif ostype.lower() == "linux":
        return get_linux_params(vmi)
    else:
        print("Sorry, {} ostype is not supported in this example yet.".format(
            ostype))
        exit(1)


def get_linux_params(vmi):
    tasks_offset = vmi.get_offset("linux_tasks")
    name_offset = vmi.get_offset("linux_name")
    pid_offset = vmi.get_offset("linux_pid")

    list_head = vmi.translate_ksym2v("init_task")

    return (tasks_offset, name_offset, pid_offset, list_head)


def get_windows_params(vmi):
    tasks_offset = vmi.get_offset("win_tasks")
    name_offset = vmi.get_offset("win_pname")
    pid_offset = vmi.get_offset("win_pid")

    list_head = vmi.read_addr_ksym("PsInitialSystemProcess")

    return (tasks_offset, name_offset, pid_offset, list_head)

def psinfo(vmi,current_addr,offset,addr_str):
    printlog('1111')
    tasks_offset, name_offset, pid_offset, list_head = get_os_params(vmi)
    current_addr = current_addr

    psname = vmi.read_str_va(current_addr + name_offset, 0)
    psid = vmi.read_32_va(current_addr + pid_offset, 0)
    print psname+'('+str(psid)+')'
    print 'addr:'+addr_str

    files = vmi.read_addr_va(current_addr+offset['files_offset'],0)
    fdt = vmi.read_addr_va(files + offset['fdt_offset'],0)
    fd = vmi.read_addr_va(fdt + offset['fd_offset'],0)
    max_fds = vmi.read_32_va(fdt + offset['maxfds_offset'], 0)
    filelist = get_files_info(vmi,fd,max_fds,offset)

def get_files_info(vmi,fd,max_fds,offset):
    printlog('2222')
    for i in range(0,max_fds):
        afile = vmi.read_addr_va(fd,0)
        fd=fd+8
        if afile!=0:
            dentry1 = vmi.read_addr_va(afile+offset['path_offset']+offset['dentry_offset'],0)
            mnt = vmi.read_addr_va(afile+offset['path_offset'],0)
            droot = vmi.read_addr_va(mnt,0)
            p_name = vmi.read_addr_va(droot+offset['dname_offset']+offset['qname_offset'],0)
            vfsname = vmi.read_str_va(p_name,0)
            p_name = vmi.read_addr_va(dentry1+offset['dname_offset']+offset['qname_offset'],0)
            filename = vmi.read_str_va(p_name,0)
            print str(i)+'&',
            pathname = filename
            if vfsname[0]=='/':
                d_parent = vmi.read_addr_va(dentry1 +offset['parent_offset'], 0)
                while d_parent!=0:
                    p_name = vmi.read_addr_va(d_parent+offset['dname_offset']+offset['qname_offset'],0)
                    d_parent = vmi.read_addr_va(d_parent +offset['parent_offset'], 0)
                    parentname = vmi.read_str_va(p_name,0)
                    if parentname=='/':
                        break
                    pathname = parentname+'/'+pathname
                print pathname
            else:
                private_date = vmi.read_addr_va(afile+offset['private_date_offset'], 0)
                sk = vmi.read_addr_va(private_date + offset['socket_sk_offset'], 0)
                #print private_date
                if filename == 'TCP' or filename == 'UDP':
                    daddr = vmi.read_32_va(sk + offset['sk_daddr_offset'], 0)
                    rcv_saddr = vmi.read_32_va(sk + offset['sk_rcv_saddr_offset'], 0)
                    sk_num = vmi.read_16_va(sk + offset['sk_num_offset'], 0)
                    sk_dport = vmi.read_16_va(sk + offset['sk_dport_offset'], 0)
                    print vfsname,pathname,numtoip(rcv_saddr),':',sk_num,'->',numtoip(daddr),':',sk_dport
                else:
                    if filename == 'TCPv6':
                        v6_daddr = vmi.read_64_va(sk + offset['sk_v6_daddr_offset'], 0)
                        v6_rcv_saddr = vmi.read_64_va(sk + offset['sk_v6_rcv_saddr_offset'], 0)
                        sk_num = vmi.read_16_va(sk + offset['sk_num_offset'], 0)
                        sk_dport = vmi.read_16_va(sk + offset['sk_dport_offset'], 0)
                        print vfsname,pathname,numtoipv6(v6_rcv_saddr),':',sk_num,'->',numtoipv6(v6_daddr),':',sk_dport
                    else:
                        print vfsname,pathname
def processelist(vmi):
    tasks_offset, name_offset, pid_offset, list_head = get_os_params(vmi)

    next_process = vmi.read_addr_va(list_head + tasks_offset, 0)
    list_head = next_process
    nump = 0
    #file_ps = open('ps', 'r')
   # ps_lines = file_ps.readlines()
    #if len(ps_lines)>0:
    #    print time.ctime(),ps_lines[len(ps_lines)-1]
    pslist = []

    while True:
        next_process = next_process-tasks_offset
        procname = vmi.read_str_va(next_process + name_offset , 0)
        pid = vmi.read_32_va(next_process + pid_offset, 0)
        #print pid,procname,hex(next_process)
        nump += 1
        
        print "%d&%s&%s"%(nump, procname,hex(next_process)[2:-1])
        next_process = vmi.read_addr_va(next_process+tasks_offset, 0)
        if (list_head == next_process):
            return nump,pslist
                

def main(domname,current_addr,addr_str):
    vmi = pyvmi.init_complete(domname)
    config_file = open('/home/vmi/Downloads/code/VmiXen/host/config/'+'4.4.57'+'_offset.conf','r')
    lines = config_file.readlines()
    config_file.close()
    offset = {}
    for line in lines:
        line_list = line.split(' = ')
        if len(line_list)==2:
            offset[line_list[0]]=int(line_list[1][:-1])
    printlog('000') 
    psinfo(vmi,current_addr,offset,addr_str)

if __name__ == "__main__":
    if len(sys.argv)==3:
        cur = sys.argv[2]
        a = int(cur,16)
        main(sys.argv[1],a,cur)
    if len(sys.argv)==2:
        vmi = pyvmi.init_complete(sys.argv[1])
        processelist(vmi)
