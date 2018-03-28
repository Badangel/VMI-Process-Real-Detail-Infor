from sock_server import *
from ext_process_list import *
from ext_module_list import *
from ext_cputime import *
from warning_log import *
import SocketServer
import time,threading,datetime
import DBHelper
import re
import pyvmi
import subprocess
import copy
import libvirt
import sys
import global_var as Globalvar

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

def Noequalzero(vmnewstat,vmoldstat):
    n = len(vmnewstat)
   # res = vmoldstat
    for i in range(0,n):
        if vmnewstat[i]!= '0':
            vmoldstat[i] = vmnewstat[i]
            #print (i),
    #print ('end')
    return vmoldstat

def quit(signum,frame):
    print "you stop"
    sys.exit()
    
def connectLibvirt(vmname):
    conn = libvirt.open('xen:///')
    if conn == None:
        print('Failed to open connection to xen:///')
        exit(1)
    domU = conn.lookupByName(vmname)
    return conn,domU
    
def getOutModuleNum(domname,module_num):
    file_module = open('tempfile/'+domname+'.module', 'r+')
    module_lines = file_module.readlines()
    for a in module_lines:
        module_apply = re.findall('\s([0-9]{1,3})\s',a)
        #print module_apply
        if len(module_apply) == 0:
            continue
        if module_apply[0] == '0':
            module_num[0]+=1
        else:
            if module_apply[0] == '1':
                module_num[1]+=1
            else:
                if module_apply[0] == '2':
                    module_num[2]+=1
                else:
                    module_num[3]+=1
    #print 'module intern num: ',len(module_lines)-1,' ',module_num
    file_module.truncate(0)

def getOutPsNum(vmi,domname,psoutlist,psouttime,hidelist):
    ps_root = 0
    ps_other = 0
    psoutstate = 0
    hidelistnew = {}
    file_ps = open('tempfile/'+domname+'.ps', 'r+')
    ps_lines = file_ps.readlines()
    print "11"
    #print ps_lines[0][0]
    if len(ps_lines)==0 or ps_lines[0][0]!='U':
        print 'ps recv error!'
        return psoutstate,ps_root,ps_other,hidelistnew
    psn = 0
    psoutstate = 1
    #print 'Libvmi','socket',len(ps_lines)
    #print psouttime,ps_lines[len(ps_lines)-1]
    b = 0
    print "22"
    for a in ps_lines:
        #print "[%3d]"%(psn),a
        psequal = 0
        psin = a.split()
        #print psin
        if a[0:4]== 'USER':
            continue
        psn+=1
        if a[0:4] == 'root':
            ps_root+=1
        else:
            ps_other+=1
        print psin[0],psin[1],psoutlist[b][1],psoutlist[b][2]
        print "33"
        while b < len(psoutlist) and psin[1]!="p" and int(psin[1]) != int(psoutlist[b][1]):
            if psoutlist[b][1] != 0 and psoutlist[b][2]!="ps" and psoutlist[b][2]!="lsmod":
                printlog(str(psoutlist[b][1])+' '+str(psoutlist[b][2])+' may hide!!')
                print "44"
                if hidelist.has_key(psoutlist[b][1]):
                    warining_w(vmi,domname,psoutlist[b][1],psoutlist[b][2])
                hidelistnew[psoutlist[b][1]]=psoutlist[b][2]
            b = b+1
            psequal = 1
        b = b+1
        '''
        if psn<=len(psoutlist) and psin[5]!=psoutlist[psn-1][2]:
            print psoutlist[psn-1],
            print psin[1],psin[5]
        if psn>len(psoutlist):
            print "out ps :",psin[1],psin[5]
        '''
    file_ps.truncate(0)
    ps_root = ps_root-1 #dec timestep     
    #print 'ps intern num: ',len(ps_lines)-2,
    return psoutstate,ps_root,ps_other,hidelistnew

def startServer():
    ser=ServerWork('223.3.69.62',9999)
    ##k=thread.start_new_thread(server_start(),ser)
    t = threading.Thread(target = ser.server_start,args =[],name='test')
    t.setDaemon(True)
    t.start()
    return ser
def stopServer(ser):
    print "shutdown port:",ser.port
    ser.server.shutdown()


def exdamain(domname,tablename):
    print "exdamain start!!!"
    try:
        i = 0
        vmname  = domname
        conn,domU = connectLibvirt(vmname)
       
        cpuoldtime = 0
        cpunowtime = 0

        db = DBHelper.DBHelper()
        vmoutlaststat = []
        vmoutstat = []
        net0old = []
        net1old = []
        vmi = pyvmi.init_complete(vmname)
        file_module = open('tempfile/'+vmname+'.module', 'r+')
        file_module.truncate()
        file_module.close()
        file_dstat = open('tempfile/'+vmname+'.d_stat', 'r+')
        file_dstat.truncate()
        file_dstat.close()
        file_ps = open('tempfile/'+vmname+'.ps', 'r+')
        file_ps.truncate()
        file_ps.close()
        hidelist = {}

        while True and Globalvar.getexit():
            try:
                #print i                
                ps_out,psoutlist = processes(vmi)
                ps_out -= 2
                psouttime = time.time()
                lsmod_out = modules(vmi)

                statmonitor_popen = subprocess.Popen(['xentop','-i 1','-nb'], stdout = subprocess.PIPE)
                outdstat = statmonitor_popen.stdout.readlines()
                
                vmnowstat = re.compile('\s([0-9.]+?)\s').findall(outdstat[2])
                net0now = re.compile('\s([0-9.]+?)[bp]+[a-z]+?\s').findall(outdstat[3])
                net1now = re.compile('\s([0-9.]+?)[bp]+[a-z]+?\s').findall(outdstat[4])
                #cpuoutuse = float(vmoutstat[1])*100
                if len(vmoutstat) == 0:
                    vmoutstat = copy.deepcopy(vmnowstat)
                    vmoutlaststat = copy.deepcopy(vmoutstat)
                    cpuoldtime = getcputime(domU)
                    net0old = net0now
                    net1old = net1now
                    continue
                #print vmnowstat
                vmoutlaststat = Noequalzero(vmoutstat,vmoutlaststat)
                vmoutstat = Noequalzero(vmnowstat,vmoutstat)
                #print vmoutstat
                #print vmoutlaststat
                cpunowtime = 0
                cpunowtime = getcputime(domU)

                use_cpu_out = min(float(cpunowtime-cpuoldtime)/10000000.0,100.0)
                #print use_cpu_out
                
                read_disk_out = max((int(vmoutstat[14]) - int(vmoutlaststat[14]))/2.0,0.0)
                write_disk_out = max((int(vmoutstat[15]) - int(vmoutlaststat[15]))/2.0,0.0)

                recv_net_out = max((int(net0now[2])+int(net1now[2])-int(net1old[2])-int(net0old[2]))/1024.0,0.0)
                recv_netp_out = max(int(net0now[3])+int(net1now[3])-int(net1old[3])-int(net0old[3]),0)
                send_net_out = max((int(net1now[0])-int(net1old[0])+int(net0now[0])-int(net0old[0]))/1024.0,0.0)
                send_netp_out = max(int(net1now[1])-int(net1old[1])+int(net0now[1])-int(net0old[1]),0)
                '''
                print net0now
                print net1now
                print recv_net_out,recv_netp_out,send_net_out,send_netp_out
                p is package
                '''
                net0old = net0now
                net1old = net1now
                #print "exs",datetime.datetime.now()
                time.sleep(0.2)
                cpuoldtime = cpunowtime
                time.sleep(0.7)
                #print "exr",datetime.datetime.now()
                file_dstat = open('tempfile/'+vmname+'.d_stat', 'r+')
                dstat_lines = file_dstat.readline()
                if len(dstat_lines) == 0:
                    print 'No data'
                    #time.sleep(0.5)
                    #continue
                    dstat_lines=" 1  1   1   1   1   1|   1     1 |   1     1 |   1     1    1 |1 1 1| 1M 1M  1M 1M|1B   1k|1  1 1|   1     1 | 1k 1M| 1  1 "
                #print re.compile('\s[0-9a-zA-Z.]+?\s').findall(dstat_lines)
                k=dstat_lines.replace('|','  ')
                vmstat = re.compile('([0-9a-zA-Z.]+?)\s').findall(k)
                #print vmstat
                file_dstat.truncate(0)
                module_num = [0,0,0,0]#relation about 0,1,2,more module
                getOutModuleNum(domname,module_num)
                psoutstate,ps_root,ps_other,hidelist = getOutPsNum(vmi,domname,psoutlist,psouttime,hidelist)
                #if psoutstate == 0:
                #   print 'ps recv error!'
                #    continue
                '''
                print ' ',ps_root,' ',ps_other
                print 'ps extern num: ',ps_out

                print 'net: ',send_net_out,stof(vmstat[21]),recv_net_out,stof(vmstat[20])
                print 'disk: ',read_disk_out,stof(vmstat[6]),write_disk_out,stof(vmstat[7])
                '''
                i=i+1
                #print "insert into state(domid:'%d',domname:'%s',usr_cpu_in:'%d',sys_cpu_in:'%d' ,idl_cpu_in:'%d',wai_cpu_in:'%d',hiq_cpu_in:'%d' ,siq_cpu_in:'%d',read_disk_in:'%f' , write_disk_in:'%f',recv_net_in:'%f',send_net_in:'%f',usd_mem_in:'%f' , buff_mem_in:'%s'->'%f' ,cach_mem_in:'%f' ,free_mem_in:'%s'->'%f' ,usd_swap_in:'%f' ,free_swap_in:'%f' ,pagein_in:'%f' ,pageout_in:'%f' ,interrupts1_in:'%d', interrupts2_in:'%d',interrupts3_in:'%d',loadavg1_in:'%f' ,loadavg5_in:'%f' ,loadavg15_in:'%f' ,int_sys_in:'%f' , csw_sys_in:'%f' ,read_total_in:'%f' , writ_total_in:'%f' , run_procs_in:'%f' ,blk_procs_in:'%f' ,new_procs_in:'%f' ,ps_root_in:'%d',ps_other_in:'%d',lsmod0_in:'%d',lsmod1_in:'%d',lsmod2_in:'%d',lsmodother_in:'%d',use_cpu_out:'%d',read_disk_out:'%f' , write_disk_out:'%f' ,recv_net_out:'%f' ,send_net_out:'%f' , lsmod_out:'%d' ,ps_out:'%d' ) "%(0,'ubuntu1604',int(vmstat[0]),int(vmstat[1]) ,int(vmstat[2]),int(vmstat[3]),int(vmstat[4]) ,int(vmstat[5]),stof(vmstat[6]) , stof(vmstat[7])  ,stof(vmstat[20])  ,stof(vmstat[21])  ,stof(vmstat[16])  , vmstat[17],stof(vmstat[17])   ,stof(vmstat[18])  ,vmstat[19],stof(vmstat[19])  ,stof(vmstat[27])  ,stof(vmstat[28])  ,stof(vmstat[8])  ,stof(vmstat[9])  ,int(vmstat[10]) , int(vmstat[11]) ,int(vmstat[12]) ,float(vmstat[13]) ,float(vmstat[14]) ,float(vmstat[15]) ,stof(vmstat[29])  , stof(vmstat[30])  ,float(vmstat[25]) , float(vmstat[26]) , float(vmstat[22]) ,float(vmstat[23]) ,float(vmstat[24]) ,ps_root  ,ps_other ,module0_num ,module1_num ,module2_num ,moduleo_num ,use_cpu_out ,read_disk_out , write_disk_out ,recv_net_out ,send_net_out , lsmod_out ,ps_out )
                #print "insert into state(domid,domname,usr_cpu_in,sys_cpu_in ,idl_cpu_in,wai_cpu_in,hiq_cpu_in ,siq_cpu_in,read_disk_in , write_disk_in ,recv_net_in ,send_net_in ,usd_mem_in , buff_mem_in ,cach_mem_in ,free_mem_in ,usd_swap_in ,free_swap_in ,pagein_in ,pageout_in ,interrupts1_in , interrupts2_in ,interrupts3_in ,loadavg1_in ,loadavg5_in ,loadavg15_in ,int_sys_in , csw_sys_in ,read_total_in , writ_total_in , run_procs_in ,blk_procs_in ,new_procs_in ,ps_root_in  ,ps_other_in ,lsmod0_in ,lsmod1_in ,lsmod2_in ,lsmodother_in ,use_cpu_out ,read_disk_out , write_disk_out ,recv_net_out ,send_net_out , lsmod_out ,ps_out ) values('%d','%s','%d','%d' ,'%d','%d','%d' ,'%d','%f' , '%f' ,'%f' ,'%f' ,'%f' , '%f' ,'%f' ,'%f' ,'%f' ,'%f' ,'%f' ,'%f' ,'%d' , '%d' ,'%d' ,'%f' ,'%f' ,'%f' ,'%f' , '%f' ,'%f' , '%f' , '%f' ,'%f' ,'%f' ,'%d'  ,'%d' ,'%d' ,'%d' ,'%d' ,'%d' ,'%d' ,'%f' , '%f' ,'%f' ,'%f' , '%d' ,'%d')"%(0,'ubuntu1604',int(vmstat[0]),int(vmstat[1]) ,int(vmstat[2]),int(vmstat[3]),int(vmstat[4]) ,int(vmstat[5]),stof(vmstat[6]) , stof(vmstat[7])  ,stof(vmstat[20])  ,stof(vmstat[21])  ,stof(vmstat[16])  , stof(vmstat[17])  ,stof(vmstat[18])  ,stof(vmstat[19])  ,stof(vmstat[27])  ,stof(vmstat[28])  ,stof(vmstat[8])  ,stof(vmstat[9])  ,int(vmstat[10]) , int(vmstat[11]) ,int(vmstat[12]) ,float(vmstat[13]) ,float(vmstat[14]) ,float(vmstat[15]) ,stof(vmstat[29])  , stof(vmstat[30])  ,float(vmstat[25]) , float(vmstat[26]) , float(vmstat[22]) ,float(vmstat[23]) ,float(vmstat[24]) ,ps_root  ,ps_other ,module0_num ,module1_num ,module2_num ,moduleo_num ,use_cpu_out ,read_disk_out , write_disk_out ,recv_net_out ,send_net_out , lsmod_out ,ps_out )
                #sql = "insert into state(domid,domname,usr_cpu_in,sys_cpu_in ,idl_cpu_in,wai_cpu_in,hiq_cpu_in ,siq_cpu_in,read_disk_in , write_disk_in ,recv_net_in ,send_net_in ,usep_mem_in,usd_mem_in , buff_mem_in ,cach_mem_in ,free_mem_in ,usep_swap_in,usd_swap_in ,free_swap_in ,pagein_in ,pageout_in ,interrupts1_in , interrupts2_in ,interrupts3_in ,loadavg1_in ,loadavg5_in ,loadavg15_in ,int_sys_in , csw_sys_in ,read_total_in , writ_total_in , run_procs_in ,blk_procs_in ,new_procs_in ,ps_root_in  ,ps_other_in ,lsmod0_in ,lsmod1_in ,lsmod2_in ,lsmodother_in ,use_cpu_out ,read_disk_out , write_disk_out ,recv_net_out ,send_net_out , lsmod_out ,ps_out,stat ) values('%d','%s','%d','%d' ,'%d','%d','%d' ,'%d','%f' , '%f' ,'%f' ,'%f' ,'%f' , '%f' ,'%f' ,'%f' ,'%f' ,'%f' ,'%f' ,'%f' ,'%d' , '%d' ,'%d' ,'%f' ,'%f' ,'%f' ,'%f' , '%f' ,'%f' , '%f' , '%f' ,'%f' ,'%f' ,'%d'  ,'%d' ,'%d' ,'%d' ,'%d' ,'%d' ,'%d' ,'%f' , '%f' ,'%f' ,'%f' , '%d' ,'%d','%d')"%(0,'ubuntu1604',int(vmstat[0]),int(vmstat[1]) ,int(vmstat[2]),int(vmstat[3]),int(vmstat[4]) ,int(vmstat[5]),stof(vmstat[6]) , stof(vmstat[7])  ,stof(vmstat[20])  ,stof(vmstat[21])  ,stof(vmstat[16])  , stof(vmstat[17])  ,stof(vmstat[18])  ,stof(vmstat[19])  ,stof(vmstat[27])  ,stof(vmstat[28])  ,stof(vmstat[8])  ,stof(vmstat[9])  ,int(vmstat[10]) , int(vmstat[11]) ,int(vmstat[12]) ,float(vmstat[13]) ,float(vmstat[14]) ,float(vmstat[15]) ,stof(vmstat[29])  , stof(vmstat[30])  ,float(vmstat[25]) , float(vmstat[26]) , float(vmstat[22]) ,float(vmstat[23]) ,float(vmstat[24]) ,ps_root  ,ps_other ,module0_num ,module1_num ,module2_num ,moduleo_num ,use_cpu_out ,read_disk_out , write_disk_out ,recv_net_out ,send_net_out , lsmod_out ,ps_out ,1)
                
                sql = "insert into "+tablename+"(domid,domname,usr_cpu_in,sys_cpu_in ,idl_cpu_in,wai_cpu_in,hiq_cpu_in ,siq_cpu_in,read_disk_in , write_disk_in ,recv_net_in ,send_net_in ,usep_mem_in,usd_mem_in , buff_mem_in ,cach_mem_in ,free_mem_in ,usep_swap_in,usd_swap_in ,free_swap_in ,pagein_in ,pageout_in ,interrupts1_in , interrupts2_in ,interrupts3_in ,loadavg1_in ,loadavg5_in ,loadavg15_in ,int_sys_in , csw_sys_in ,read_total_in , writ_total_in , run_procs_in ,blk_procs_in ,new_procs_in ,ps_root_in  ,ps_other_in ,lsmod0_in ,lsmod1_in ,lsmod2_in ,lsmodother_in ,use_cpu_out ,read_disk_out , write_disk_out ,recv_net_out ,recv_netp_out,send_net_out,send_netp_out , lsmod_out ,ps_out,stat ) values('%d','%s','%d','%d' ,'%d','%d','%d' ,'%d','%f' , '%f' ,'%f' ,'%f','%d' ,'%f' , '%f' ,'%f' ,'%f' ,'%d','%f' ,'%f' ,'%f' ,'%f' ,'%d' , '%d' ,'%d' ,'%f' ,'%f' ,'%f' ,'%f' , '%f' ,'%f' , '%f' , '%f' ,'%f' ,'%f' ,'%d'  ,'%d' ,'%d' ,'%d' ,'%d' ,'%d' ,'%d' ,'%f' , '%f' ,'%f' ,'%d','%f', '%d', '%d' ,'%d','%d')"%(0,domname,int(vmstat[0]),int(vmstat[1]) ,int(vmstat[2]),int(vmstat[3]),int(vmstat[4]) ,int(vmstat[5]),stof(vmstat[6]) , stof(vmstat[7]) ,stof(vmstat[20])  ,stof(vmstat[21]),100*stof(vmstat[16])/(0.1+stof(vmstat[16])+stof(vmstat[17])+stof(vmstat[18])+stof(vmstat[19])) ,stof(vmstat[16])  , stof(vmstat[17])  ,stof(vmstat[18])  ,stof(vmstat[19])  ,100*stof(vmstat[27])/(0.1+stof(vmstat[27])+stof(vmstat[28])),stof(vmstat[27])  ,stof(vmstat[28])  ,stof(vmstat[8])  ,stof(vmstat[9])  ,int(vmstat[10]) , int(vmstat[11]) ,int(vmstat[12]) ,float(vmstat[13]) ,float(vmstat[14]) ,float(vmstat[15]) ,stof(vmstat[29])  , stof(vmstat[30])  ,float(vmstat[25]) , float(vmstat[26]) , float(vmstat[22]) ,float(vmstat[23]) ,float(vmstat[24]) ,ps_root  ,ps_other ,module_num[0] ,module_num[1] ,module_num[2] ,module_num[3] ,use_cpu_out ,read_disk_out , write_disk_out ,recv_net_out ,recv_netp_out ,send_net_out ,send_netp_out , lsmod_out ,ps_out ,0)
                
                if Globalvar.getexit():
                    data = db.oncesql(sql)
                    printlog("add 1 to "+tablename)
                    
            except IndexError:
                print "list out range"
                continue
        #stopServer()
    except Exception:
        print "exdamain fail"

if __name__ =='__main__':
    try:
        signal.signal(signal.SIGINT,quit)
        signal.signal(signal.SIGTERM,quit) 
        startServer()
        exdamain(sys.argv[1])
    except Exception:
        print "over"
    