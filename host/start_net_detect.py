#!/usr/bin/python
import threading,time,sys,re
import DBSnort
from state.DBHelper import DBHelper
from state.vmaddr_config import vmaddr
from queue import Queue
from state.warning_log import *

def numtoip(num):
    ch2 = lambda x: '.'.join([str(x/(256**i)%256) for i in range(3,-1,-1)])
    return ch2(num)
def iptonum(ip):
    ch3 = lambda x:sum([256**j*int(i) for j,i in enumerate(x.split('.')[::-1])])
    return ch3(ip)

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
    sql = "select event.cid,signature.sig_name,iphdr.ip_src,iphdr.ip_dst,tcphdr.tcp_sport,tcphdr.tcp_dport from event,iphdr,signature,tcphdr where event.timestamp>=(now()-interval 10 hour) and event.state=0 and tcphdr.cid = event.cid and (iphdr.ip_src="+str(domaddrnum)+" or iphdr.ip_dst="+str(domaddrnum)+") and event.cid = iphdr.cid and event.signature = signature.sig_id; "
    while True:
        ipeventdata = db.oncesql(sql)
        ipeventdata = list(ipeventdata)
        i = 0
        for a in ipeventdata:
            i=i+1
            saddr = numtoip(a[2])
            daddr = numtoip(a[3])
            print i,a[0],a[1],numtoip(a[2])+':'+str(a[4])+'->'+numtoip(a[3])+':'+str(a[5])
            warningsql = "insert into warning(domname,class,sqlid,saddr,daddr,sport,dport) values('%s','%s','%d','%s','%s','%d','%d')"%(domname,"Net Anomaly",a[0],saddr,daddr,a[4],a[5])
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