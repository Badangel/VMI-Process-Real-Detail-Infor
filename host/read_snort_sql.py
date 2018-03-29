#!/usr/bin/python
import DBSnort
from state.DBHelper import DBHelper
from state.vmaddr_config import vmaddr
import time
import signal
import re,sys
def numtoip(num):
    ch2 = lambda x: '.'.join([str(x/(256**i)%256) for i in range(3,-1,-1)])
    return ch2(num)
def iptonum(ip):
    ch3 = lambda x:sum([256**j*int(i) for j,i in enumerate(x.split('.')[::-1])])
    return ch3(ip)

def get_all_event(mincid):
    db = DBSnort.DBSnort()
    sql = "select event.cid,event.signature,signature.sig_name,iphdr.ip_src,iphdr.ip_dst from event,iphdr,signature where event.cid > "+mincid+" and event.cid = iphdr.cid and event.signature = signature.sig_id"
    ipeventdata = db.oncesql(sql)
    ipeventdata = list(ipeventdata)
    i = 0
    for a in ipeventdata:
        i=i+1
        print i,a[0],a[1],a[2],numtoip(a[3]),numtoip(a[4])       
    return mincid

def get_event(mincid,iplist,dbwarning):
    db = DBSnort.DBSnort()
    sql = "select event.cid,event.signature,signature.sig_name,iphdr.ip_src,iphdr.ip_dst from event,iphdr,signature where event.cid > "+mincid+" and event.cid = iphdr.cid and event.signature = signature.sig_id"
    ipeventdata = db.oncesql(sql)
    ipeventdata = list(ipeventdata)
    i = 0
    for a in ipeventdata:
        saddr = numtoip(a[3])
        daddr = numtoip(a[4])
        if (saddr in iplist) or (daddr in iplist):
            i = i + 1
            print i,a[0],a[1],a[2],numtoip(a[3]),numtoip(a[4])
            vmname = ""
            if(vmaddr.has_key(saddr)):
                vmname = vmaddr[saddr]
            else:
                vmname = vmaddr[daddr]
            warningsql = "insert into warning(domname,class,sqlid,saddr,daddr) values('%s','%s','%d','%s','%s')"%(vmname,a[2],a[0],saddr,daddr)
            dbwarning.oncesql(warningsql)
           
        mincid = str(a[0]) 
     
    write_sql_cid(mincid)          
    return mincid

def get_tcp_event(mincid,iplist,dbwarning):
    db = DBSnort.DBSnort()
    sql = "select event.cid,event.signature,signature.sig_name,iphdr.ip_src,iphdr.ip_dst,tcphdr.tcp_sport,tcphdr.tcp_dport from event,iphdr,signature,tcphdr where event.cid > "+mincid+" and event.cid = iphdr.cid and event.signature = signature.sig_id and tcphdr.cid = event.cid"
    ipeventdata = db.oncesql(sql)
    ipeventdata = list(ipeventdata)
    i = 0
    for a in ipeventdata:
        saddr = numtoip(a[3])
        daddr = numtoip(a[4])
        if (saddr in iplist) or (daddr in iplist):
            i = i + 1
            print i,a[0],a[1],a[2],numtoip(a[3])+':'+str(a[5])+'->'+numtoip(a[4])+':'+str(a[6])
            vmname = ""
            if(vmaddr.has_key(saddr)):
                vmname = vmaddr[saddr]
            else:
                vmname = vmaddr[daddr]
            warningsql = "insert into warning(domname,class,sqlid,saddr,daddr,sport,dport) values('%s','%s','%d','%s','%s','%d','%d')"%(vmname,a[2],a[0],saddr,daddr,a[5],a[6])
            dbwarning.oncesql(warningsql)
        mincid = str(a[0])
    write_sql_cid(mincid)
    return mincid

def initvmip():
    iplist = []
    for key in vmaddr:
        iplist.append(key)
    print iplist
    return iplist

def write_sql_cid(sqlcid):
    file_snortcid = open('tempfile/snort.sql', 'w+')
    file_snortcid.write(sqlcid)
    file_snortcid.close()
def main():
    iplist = initvmip()
    file_snortcid = open('tempfile/snort.sql', 'r+')
    sqlcid = file_snortcid.read()
    file_snortcid.close()
        
    dbwarning = DBHelper()
    while True:
        sqlcid = get_event(sqlcid,iplist,dbwarning)
        print time.ctime()
        time.sleep(1)
        

if __name__ =='__main__':
    #get_all_event(str(518))
    main()