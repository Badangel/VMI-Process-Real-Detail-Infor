#!/usr/bin/python
import threading,time,sys,re
import DBSnort
from state.DBHelper import DBHelper
from state.vmaddr_config import vmaddr
from queue import Queue
from state.warning_log import *
import pyvmi
import socket

def numtoip(num):
    ch2 = lambda x: '.'.join([str(x/(256**i)%256) for i in range(3,-1,-1)])
    return ch2(num)
def iptonum(ip):
    ch3 = lambda x:sum([256**j*int(i) for j,i in enumerate(x.split('.')[::-1])])
    return ch3(ip)

def read_offset(domname):
    config_file = open('/home/vmi/Downloads/code/VmiXen/host/config/'+'4.4.57'+'_offset.conf','r')
    lines = config_file.readlines()
    config_file.close()
    offset = {}
    for line in lines:
        line_list = line.split(' = ')
        if len(line_list)==2:
            offset[line_list[0]]=int(line_list[1][:-1])
    return offset

def get_ps_port_list(vmi,fdt,offset,domaddr):
    fd = vmi.read_addr_va(fdt + offset['fd_offset'],0)
    max_fds = vmi.read_32_va(fdt + offset['maxfds_offset'], 0)
    ps_port_list=[]
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
            
            pathname = filename
            if vfsname[0]=='/':
                continue
            else:
                private_date = vmi.read_addr_va(afile+offset['private_date_offset'], 0)
                sk = vmi.read_addr_va(private_date + offset['socket_sk_offset'], 0)
                #print private_date
                if filename == 'TCP' or filename == 'UDP':
                    daddr = vmi.read_32_va(sk + offset['sk_daddr_offset'], 0)
                    rcv_saddr = vmi.read_32_va(sk + offset['sk_rcv_saddr_offset'], 0)
                    sk_num = vmi.read_16_va(sk + offset['sk_num_offset'], 0)
                    sk_dport = vmi.read_16_va(sk + offset['sk_dport_offset'], 0)
                    rcv_saddr=socket.htonl(rcv_saddr)
                    daddr=socket.htonl(daddr)
                    print vfsname,pathname,numtoip(rcv_saddr),':',sk_num,'->',numtoip(daddr),':',sk_dport
                    if numtoip(daddr)==domaddr:
                        ps_port_list.append(sk_dport)
                    if numtoip(rcv_saddr)==domaddr:
                        ps_port_list.append(sk_num)
                else:
                    continue
    return ps_port_list

def get_vm_port_list(domname,domaddr,offset):
    vmi = pyvmi.init_complete(sys.argv[1])
    tasks_offset = vmi.get_offset("linux_tasks")
    name_offset = vmi.get_offset("linux_name")
    pid_offset = vmi.get_offset("linux_pid")

    list_head = vmi.translate_ksym2v("init_task")
    next_process = vmi.read_addr_va(list_head + tasks_offset, 0)
    list_head = next_process
    nump = 0
    #file_ps = open('ps', 'r')
   # ps_lines = file_ps.readlines()
    #if len(ps_lines)>0:
    #    print time.ctime(),ps_lines[len(ps_lines)-1]
    pslist = []
    socketport=[]
    while True:
        next_process = next_process-tasks_offset
        procname = vmi.read_str_va(next_process + name_offset , 0)
        pid = vmi.read_32_va(next_process + pid_offset, 0)
        #print pid,procname,hex(next_process)
        nump += 1
        current_addr = next_process

        psname = vmi.read_str_va(current_addr + name_offset, 0)
        psid = vmi.read_32_va(current_addr + pid_offset, 0)
        
        print "%d&%s&%s"%(pid, procname,hex(next_process)[2:-1])

        files = vmi.read_addr_va(current_addr+offset['files_offset'],0)
        if files!=0:
            fdt = vmi.read_addr_va(files + offset['fdt_offset'],0)
            ps_port_list = get_ps_port_list(vmi,fdt,offset,domaddr)
            for port in ps_port_list:
                socketport.append([pid,procname,port])

        next_process = vmi.read_addr_va(next_process+tasks_offset, 0)
        if (list_head == next_process):
            print 'over'
            break
        
    return socketport

def get_port_to_ps(port,socketport):
    for a in socketport:
        if a[2]==port:
            return a[0],a[1]
    return 0,0

def start_ip(domname,in_q):
    dbwarning = DBHelper.DBHelper()
    db = DBSnort.DBSnort()
    domaddr = ""
    for i in vmaddr:
        if cmp(vmaddr[i],domname)==0:
            domaddr = i
            break
    domaddrnum = iptonum(domaddr)

    getnetfactor_sql = "select net_p,net_m,net_c,net_anomaly from response where domname = '"+domname+"'"
    netfactor = dbwarning.oncesql(getnetfactor_sql)
    
    netfactor1 = list(netfactor)
    nfactor = netfactor1[0]
    print nfactor
    net_p = int(nfactor[0])
    net_m = int(nfactor[1])
    net_c = int(nfactor[2])
    net_f = int(nfactor[3])
    factortotal = 0

    sql = "select event.cid,signature.sig_name,iphdr.ip_src,iphdr.ip_dst from event,iphdr,signature where event.timestamp>=(now()-interval 10 hour) and event.state=0 and (iphdr.ip_src="+str(domaddrnum)+" or iphdr.ip_dst="+str(domaddrnum)+") and event.cid = iphdr.cid and event.signature = signature.sig_id and event.cid not in (select cid from tcphdr)"
    while True:
        ipeventdata = db.oncesql(sql)
        ipeventdata = list(ipeventdata)
        i = 0
        for a in ipeventdata:
            i=i+1
            saddr = numtoip(a[2])
            daddr = numtoip(a[3])
            factortotal = factortotal + net_f
            warningsql = "insert into warning(domname,class,sqlid,saddr,daddr) values('%s','%s','%d','%s','%s')"%(domname,"Net Anomaly",a[0],saddr,daddr)
            dbwarning.oncesql(warningsql)
            updatenet = "update event set state = 1 where cid = "+str(a[0])
            db.oncesql(updatenet)
        while not in_q.empty():
            qdata = in_q.get()
            print "queue get data:",qdata,"ip warning:",i
            factortotal = factortotal + net_f*qdata
        setfactor_sql="update response set net_factor = "+str(factortotal)+" where domname = '"+domname+"'"
        dbwarning.oncesql(setfactor_sql)
        if factortotal>net_p:
            responsevm(domname,1)
        else:
            if factortotal>net_c:
                responsevm(domname,2)
            else:
                if factortotal>net_m:
                    responsevm(domname,3)
        net_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.netdetect', 'r')
        net_Det = int(net_detect.readline())
        net_detect.close()
        if net_Det==0:
            remusefactor_sql="update response set net_factor = 0 where domname = '"+domname+"'"
            dbwarning.oncesql(remusefactor_sql)
            break
        time.sleep(1)
    

def start_tcp(domname,out_q):
    dbwarning = DBHelper.DBHelper()
    db = DBSnort.DBSnort()
    domaddr = ""
    for i in vmaddr:
        if cmp(vmaddr[i],domname)==0:
            domaddr = i
            break
    domaddrnum = iptonum(domaddr)

    offset = read_offset(domname)
    socketport=[]

    sql = "select event.cid,signature.sig_name,iphdr.ip_src,iphdr.ip_dst,tcphdr.tcp_sport,tcphdr.tcp_dport from event,iphdr,signature,tcphdr where event.timestamp>=(now()-interval 10 hour) and event.state=0 and tcphdr.cid = event.cid and (iphdr.ip_src="+str(domaddrnum)+" or iphdr.ip_dst="+str(domaddrnum)+") and event.cid = iphdr.cid and event.signature = signature.sig_id; "
    while True:
        ipeventdata = db.oncesql(sql)
        ipeventdata = list(ipeventdata)
        i = 0

        if len(ipeventdata)>0:
            socketport = get_vm_port_list(domname,domaddr,offset)

        for a in ipeventdata:
            i=i+1
            saddr = numtoip(a[2])
            daddr = numtoip(a[3])
            print i,a[0],a[1],numtoip(a[2])+':'+str(a[4])+'->'+numtoip(a[3])+':'+str(a[5])
            
            psid = -1
            psname = ""
            if numtoip(a[2]) == domaddr:
                psid,psname = get_port_to_ps(a[4],socketport)
            if numtoip(a[3]) == domaddr:
                psid,psname = get_port_to_ps(a[5],socketport)

            warningsql = "insert into warning(domname,class,sqlid,psid,psname,saddr,daddr,sport,dport) values('%s','%s','%d','%d','%s','%s','%s','%d','%d')"%(domname,"Net Anomaly",a[0],psid,psname,saddr,daddr,a[4],a[5])
            dbwarning.oncesql(warningsql)
            updatenet = "update event set state = 1 where cid = "+str(a[0])
            db.oncesql(updatenet)
        out_q.put(i)
        net_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.netdetect', 'r')
        net_Det = int(net_detect.readline())
        net_detect.close()
        if net_Det==0:
            break
        time.sleep(1)
   


def start_net(domname):
    q = Queue()
    t0 = threading.Thread(target = start_ip,args =[domname,q],name='start_ip')
    t0.setDaemon(False)
    t0.start()
         
    t1 = threading.Thread(target = start_tcp,args =[domname,q],name='start_tcp')
    t1.setDaemon(False)
    t1.start()   
    
    printlog("net start over!!")

def test(domname):
    domaddr = ""
    for i in vmaddr:
        if cmp(vmaddr[i],domname)==0:
            domaddr = i
            break
    domaddrnum = iptonum(domaddr)
    print 'ip:',domaddr
    offset = read_offset(domname)
    print get_vm_port_list(domname,domaddr,offset)

if __name__=='__main__':
    domname = sys.argv[1]
    
    net_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.netdetect', 'r')
    net_Det = net_detect.readline()
    net_detect.close()
    print int(net_Det)
    if int(net_Det)==0:
        printlog("net detect will start !! "+net_Det)
        net_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.netdetect', 'w')
        net_detect.write("1")
        net_detect.close()
        start_net(domname)
    