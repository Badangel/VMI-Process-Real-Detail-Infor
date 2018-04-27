#!/usr/bin/python

import subprocess,time,paramiko,threading
import state.global_var as Globalvar
from run_psinfo import runpsinfo

def vm_renew():
    creataddimgshell = 'qemu-img create -b /home/vmi/Downloads/ubuntu1604.img -f qcow2 /home/vmi/Downloads/test.qcow2 10G'
    creataddimg = subprocess.Popen(str(creataddimgshell), stdout=subprocess.PIPE, shell=True)
    creataddimg.wait()
    print 'creat addimg over!'
    time.sleep(2)
    chmodvm = subprocess.Popen(['chmod 777 /home/vmi/Downloads/test.qcow2'], stdout=subprocess.PIPE, shell=True)
    chmodvm.wait()
    print 'chmod vm over!'
    time.sleep(2)
    startvm = subprocess.Popen(['virsh start ubuntu1604add'], stdout=subprocess.PIPE, shell=True)
    startvm.wait()
    print 'start vm over!'
    time.sleep(30)

def ssh_connect():
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    ssh.connect(hostname='223.3.71.214', port=22, username='ubuntu1604', password='309911')
    print 'ssh ok!'
    return ssh

def ssh_cmd(sshc,runVS):
    cmd = '/home/ubuntu1604/Downloads/malware/vsup/'+runVS
    sshc.exec_command(cmd)
    

def ssh_close(sshc):
    sshc.close()

def vm_shutdown():
    closevm = subprocess.Popen(['virsh destroy ubuntu1604add'], stdout=subprocess.PIPE, shell=True)
    closevm.wait()
    print 'close vm over!'
    time.sleep(3)
    deleteimg = subprocess.Popen(['rm -f /home/vmi/Downloads/test.qcow2'], stdout=subprocess.PIPE, shell=True)
    deleteimg.wait()
    time.sleep(5)
    print 'delete vm over!'

if __name__ =='__main__':
    n = 44
    vslist = []
    vslistfile = open('/home/vmi/Downloads/code/VmiXen/host/vslist', 'r+')
    vslistname = vslistfile.readlines()
    for a in vslistname:
        a1 = a[:-1]
        vslist.append(a1)
    
    while n<252:
        domname = 'ubuntu1604add'
        
        Globalvar.setexitT()
        vm_renew()
        
        sshc = ssh_connect()
        time.sleep(2)

        t1 = threading.Thread(target = runpsinfo,args =[domname,'2'],name='getpsinfo')
        t1.setDaemon(True)
        t1.start()
        print 'run get psinfo!'
        time.sleep(2)

        ssh_cmd(sshc,vslist[n])
        print 'run vs!',vslist[n]
        time.sleep(10)
        Globalvar.setexit()
        ssh_close(sshc)
        time.sleep(2)
        vm_shutdown()
        
        print n,vslist[n],'collect over!'
        n=n+1
    print 'all over'

