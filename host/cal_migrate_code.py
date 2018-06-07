#!/usr/bin/python
import socket,time,re,math
from state.DBHelper import DBHelper

def stof(num):
    if num[-1] == "k":
        res = float(num[:-1])
        return res
    else:
        if num[-1] == 'M':
            res = float(num[:-1])*1024
            return res
        else:
            if num[-1] == 'B':
                num = num[:-1]
            res = float(num)/1024.0
            return res

def get_remote_info(ip,port):
    s = socket.socket()
    while True:
        try:
            #s.settimeout(1)
            s.connect((ip,port))
            #print "connect success!"
            start_time = time.time()
            s.send('q')
            #get dstat value
            dstat = s.recv(1024)
            end_time = time.time()
            ttl = end_time-start_time
            print 'ttl:',ttl
            return dstat,ttl
        except:
            print "connect fail, again"
            return '',-1
            time.sleep(1)

def get_vm_info(domname):
    vm_file = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.d_stat','r')
    data = vm_file.readline()
    vm_file.close()
    k=data.replace('|','  ')
    vmstat = re.compile('([0-9a-zA-Z.]+?)\s').findall(k)
    cpu_use = 100-int(vmstat[2])*1.0
    mem_use = float(vmstat[16][:-1])*100.0/(float(vmstat[16][:-1])+float(vmstat[17][:-1])+float(vmstat[18][:-1])+float(vmstat[19][:-1]))
    swap_use = float(vmstat[27][:-1])*100.0/(float(vmstat[27][:-1])+float(vmstat[28][:-1]))
    net_speed = (stof(vmstat[20])+stof(vmstat[21]))/1024.0
    disk_io = float(vmstat[25])+float(vmstat[26])
    return cpu_use,mem_use,swap_use,net_speed,disk_io

def host_factor(domname,ip):
    vm_cpu_use,vm_mem_use,vm_swap_use,vm_net_speed,vm_disk_io = get_vm_info(domname)
    #print vm_cpu_use,vm_mem_use,vm_swap_use,vm_net_speed,vm_disk_io
    data,ttl = get_remote_info(ip,9998)
    if ttl==-1:
        return -1,-1
    #print data
    host_data = data.split(',')
    host_cpu_use = float(host_data[0][2:])
    host_mem_use = float(host_data[1])
    host_swap_use = float(host_data[2])
    host_net_reci_use = float(host_data[3])
    host_net_send_use= float(host_data[4])
    host_disk_use_next = float(host_data[5])
    host_disk_io  = float(host_data[6][:-2])
    host_net_speed = host_net_reci_use + host_net_send_use
    #print host_cpu_use,host_mem_use,host_swap_use,host_net_speed,host_disk_io

    vm_mod = math.sqrt(pow(vm_cpu_use,2)+pow(vm_mem_use,2)+pow(vm_net_speed,2)+pow(vm_disk_io,2))
    host_mod = math.sqrt(pow(host_cpu_use,2)+pow(host_mem_use,2)+pow(host_net_speed,2)+pow(host_disk_io,2))

    performance = (vm_cpu_use*host_cpu_use+vm_mem_use*host_mem_use+vm_net_speed*host_net_speed+vm_disk_io*host_disk_io)/(vm_mod*host_mod)
    print performance

    migrate_cost = host_mem_use*ttl
    return performance,migrate_cost

def migrate_score(domname):
    a_mcf = 1
    a_pf = 1

    selectdb = DBHelper()
    sqlstate = "select ip,classier from iptable" 
    ipdata = selectdb.oncesql(sqlstate)
    ipdata1 = list(ipdata)
    migrate_ip = ''
    min_score = float('inf')
    for ipone in ipdata1:
        security = 0
        if ipone[1]=='1':
            security=float('inf')
            continue
        pscore,migrate_cost = host_factor(domname,ipone[0])
        cost = a_pf*pscore+a_mcf*migrate_cost
        if score==-1:
            print ipone[0],'host lost'
            continue
        if score < min_score:
            migrate_ip = ipone[0]
            min_score = score

        print 'factor:',ipone[0],score
    return migrate_ip,min_score

if __name__=='__main__':
    domname = 'ubuntu1604test'
    print migrate_score(domname)

