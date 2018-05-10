#!/usr/bin/python
import threading,time,sys,re
import DBSnort
from state.DBHelper import DBHelper
from state.vmaddr_config import vmaddr

from state.warning_log import *

def numtoip(num):
    ch2 = lambda x: '.'.join([str(x/(256**i)%256) for i in range(3,-1,-1)])
    return ch2(num)
def iptonum(ip):
    ch3 = lambda x:sum([256**j*int(i) for j,i in enumerate(x.split('.')[::-1])])
    return ch3(ip)

def start_ip(domname):
    dbwarning = DBHelper.DBHelper()
    db = DBSnort.DBSnort()
    domaddr = ""
    for i in vmaddr:
        if cmp(vmaddr[i],domname)==0:
            domaddr = i
            break
    domaddrnum = iptonum(domaddr)

    sql = "select event.cid,signature.sig_name,iphdr.ip_src,iphdr.ip_dst from event,iphdr,signature where event.timestamp>=(now()-interval 10 hour) and (iphdr.ip_src="+str(domaddrnum)+" or iphdr.ip_dst="+str(domaddrnum)+") and event.cid = iphdr.cid and event.signature = signature.sig_id and event.cid not in (select cid from tcphdr)"
    while True:
        ipeventdata = db.oncesql(sql)
        ipeventdata = list(ipeventdata)
        i = 0
        for a in ipeventdata:
            saddr = numtoip(a[2])
            daddr = numtoip(a[3])
            warningsql = "insert into warning(domname,class,sqlid,saddr,daddr) values('%s','%s','%d','%s','%s')"%(domname,"Net Anomaly",a[0],saddr,daddr)
            dbwarning.oncesql(warningsql)
        net_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.netdetect', 'r')
        net_Det = int(net_detect.readline())
        net_detect.close()
        if net_Det==0:
            break
        time.sleep(1)
    

def start_tcp(domname):
    dbwarning = DBHelper.DBHelper()
    db = DBSnort.DBSnort()
    domaddr = ""
    for i in vmaddr:
        if cmp(vmaddr[i],domname)==0:
            domaddr = i
            break
    domaddrnum = iptonum(domaddr)
    sql = "select event.cid,signature.sig_name,iphdr.ip_src,iphdr.ip_dst,tcphdr.tcp_sport,tcphdr.tcp_dport from event,iphdr,signature,tcphdr where event.timestamp>=(now()-interval 10 hour) and tcphdr.cid = event.cid and (iphdr.ip_src="+str(domaddrnum)+" or iphdr.ip_dst="+str(domaddrnum)+") and event.cid = iphdr.cid and event.signature = signature.sig_id; "
    while True:
        ipeventdata = db.oncesql(sql)
        ipeventdata = list(ipeventdata)
        i = 0
        for a in ipeventdata:
            saddr = numtoip(a[2])
            daddr = numtoip(a[3])
            print i,a[0],a[1],numtoip(a[2])+':'+str(a[4])+'->'+numtoip(a[3])+':'+str(a[5])
            warningsql = "insert into warning(domname,class,sqlid,saddr,daddr,sport,dport) values('%s','%s','%d','%s','%s','%d','%d')"%(domname,"Net Anomaly",a[0],saddr,daddr,a[4],a[5])
            dbwarning.oncesql(warningsql)
        net_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.netdetect', 'r')
        net_Det = int(net_detect.readline())
        net_detect.close()
        if net_Det==0:
            break
        time.sleep(1)
   


def start_net(domname):
    t0 = threading.Thread(target = start_ip,args =[domname],name='start_ip')
    t0.setDaemon(False)
    t0.start()
         
    t1 = threading.Thread(target = start_tcp,args =[domname],name='start_tcp')
    t1.setDaemon(False)
    t1.start()   
    
    printlog("net start over!!")


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