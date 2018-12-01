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
    file_module = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.module', 'r+')
    module_lines = file_module.readlines()
    #file_module.truncate(0)
    file_module.close()
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
    

def getOutPsNum(vmi,domname,psoutlist,psouttime,hidelist):
    ps_root = 0
    ps_other = 0
    psoutstate = 0
    hidelistnew = {}
    file_ps = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.ps', 'r+')
    ps_lines = file_ps.readlines()
    #file_ps.truncate(0)
    file_ps.close()
    #print ps_lines[0][0]
    psintime = ps_lines[len(ps_lines)-1].split(" ")
    ret = 0
    print psouttime,ps_lines[len(ps_lines)-1],psintime[0]
    if psintime[0].find(".")!=-1 and abs(psouttime-float(psintime[0]))<=2:
        ret = 1
    if ret==0 or len(ps_lines)==0 or ps_lines[0][0]!='U':
        print 'get ps recv error!'
        return psoutstate,ps_root,ps_other,hidelistnew
    psn = 0
    psoutstate = 1
    #print 'Libvmi','socket',len(ps_lines)
    
    b = 0
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
        #print psin[0],psin[1],psoutlist[b][1],psoutlist[b][2]
      
        while b < len(psoutlist) and psin[1]!="p" and int(float(psin[1])) != int(psoutlist[b][1]):
            if psoutlist[b][1] != 0 and psoutlist[b][2]!="ps" and psoutlist[b][2]!="lsmod":
                printlog(str(psoutlist[b][1])+' '+str(psoutlist[b][2])+' may hide!!')
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
        file_module = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+vmname+'.module', 'r+')
        file_module.truncate()
        file_module.close()
        file_dstat = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+vmname+'.d_stat', 'r+')
        file_dstat.truncate()
        file_dstat.close()
        file_ps = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+vmname+'.ps', 'r+')
        file_ps.truncate()
        file_ps.close()
        hidelist = {}

        state_detect1 = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.statedetect', 'r')
        state_Det1 = state_detect1.readline()
        state_detect1.close()
        while int(state_Det1)==1:
            time.sleep(1)
	    state_detect1 = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.statedetect', 'r')
	    state_Det1 = state_detect1.readline()
	    state_detect1.close()
        while True :
            try:
                print i                
                ps_out,psoutlist = processes(vmi)
                print "11"
                ps_out -= 2
                psouttime = time.time()
                lsmod_out = modules(vmi)

                statmonitor_popen = subprocess.Popen(['xentop','-i 1','-nb'], stdout = subprocess.PIPE)
                outdstat = statmonitor_popen.stdout.readlines()
                
                print "222"
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
                    printlog("ex continue")
                    continue
                #print vmnowstat
                vmoutlaststat = Noequalzero(vmoutstat,vmoutlaststat)
                vmoutstat = Noequalzero(vmnowstat,vmoutstat)
                #print vmoutstat
                #print vmoutlaststat
                cpunowtime = 0
                cpunowtime = getcputime(domU)

                print "333"

                use_cpu_out = min(float(cpunowtime-cpuoldtime)/10000000.0,100.0)
                #print use_cpu_out
                
                read_disk_out = max((int(vmoutstat[14]) - int(vmoutlaststat[14]))/2.0,0.0)
                write_disk_out = max((int(vmoutstat[15]) - int(vmoutlaststat[15]))/2.0,0.0)

                recv_net_out = max((int(net0now[2])+int(net1now[2])-int(net1old[2])-int(net0old[2]))/1024.0,0.0)
                recv_netp_out = max(int(net0now[3])+int(net1now[3])-int(net1old[3])-int(net0old[3]),0)
                send_net_out = max((int(net1now[0])-int(net1old[0])+int(net0now[0])-int(net0old[0]))/1024.0,0.0)
                send_netp_out = max(int(net1now[1])-int(net1old[1])+int(net0now[1])-int(net0old[1]),0)
                
                net0old = net0now
                net1old = net1now
                #print "exs",datetime.datetime.now()
                time.sleep(0.2)
                cpuoldtime = cpunowtime
                time.sleep(0.7)
                #print "exr",datetime.datetime.now()
                file_dstat = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+vmname+'.d_stat', 'r')
                dstat_lines = file_dstat.readline()
                if len(dstat_lines) == 0:
                    printlog('No data')
                    #time.sleep(0.5)
                    #continue
                    dstat_lines=" 1  1   1   1   1   1|   1     1 |   1     1 |   1     1    1 |1 1 1| 1M 1M  1M 1M|1B   1k|1  1 1|   1     1 | 1k 1M| 1  1 "
                
                file_dstat.close()
                print "444"
                #print re.compile('\s[0-9a-zA-Z.]+?\s').findall(dstat_lines)
                
                #print vmstat
                               
                print "555"
                psoutstate,ps_root,ps_other,hidelist = getOutPsNum(vmi,domname,psoutlist,psouttime,hidelist)
                module_num = [0,0,0,0]#relation about 0,1,2,more module
                print "5551"
                getOutModuleNum(domname,module_num)
                print "666"
                if ps_root==0:
                    dstat_lines = " 1  1   1   1   1   1|   1     1 |   1     1 |   1     1    1 |1 1 1| 1M 1M  1M 1M|1B   1k|1  1 1|   1     1 | 1k 1M| 1  1 "
                    module_num = [0,0,0,0]
                k=dstat_lines.replace('|','  ')
                vmstat = re.compile('([0-9a-zA-Z.]+?)\s').findall(k)
                i=i+1
                
                sql = "insert into "+tablename+"(domid,domname,usr_cpu_in,sys_cpu_in ,idl_cpu_in,wai_cpu_in,hiq_cpu_in ,siq_cpu_in,read_disk_in , write_disk_in ,recv_net_in ,send_net_in ,usep_mem_in,usd_mem_in , buff_mem_in ,cach_mem_in ,free_mem_in ,usep_swap_in,usd_swap_in ,free_swap_in ,pagein_in ,pageout_in ,interrupts1_in , interrupts2_in ,interrupts3_in ,loadavg1_in ,loadavg5_in ,loadavg15_in ,int_sys_in , csw_sys_in ,read_total_in , writ_total_in , run_procs_in ,blk_procs_in ,new_procs_in ,ps_root_in  ,ps_other_in ,lsmod0_in ,lsmod1_in ,lsmod2_in ,lsmodother_in ,use_cpu_out ,read_disk_out , write_disk_out ,recv_net_out ,recv_netp_out,send_net_out,send_netp_out , lsmod_out ,ps_out,stat ) values('%d','%s','%d','%d' ,'%d','%d','%d' ,'%d','%f' , '%f' ,'%f' ,'%f','%d' ,'%f' , '%f' ,'%f' ,'%f' ,'%d','%f' ,'%f' ,'%f' ,'%f' ,'%d' , '%d' ,'%d' ,'%f' ,'%f' ,'%f' ,'%f' , '%f' ,'%f' , '%f' , '%f' ,'%f' ,'%f' ,'%d'  ,'%d' ,'%d' ,'%d' ,'%d' ,'%d' ,'%d' ,'%f' , '%f' ,'%f' ,'%d','%f', '%d', '%d' ,'%d','%d')"%(0,domname,int(vmstat[0]),int(vmstat[1]) ,int(vmstat[2]),int(vmstat[3]),int(vmstat[4]) ,int(vmstat[5]),stof(vmstat[6]) , stof(vmstat[7]) ,stof(vmstat[20])  ,stof(vmstat[21]),100*stof(vmstat[16])/(0.1+stof(vmstat[16])+stof(vmstat[17])+stof(vmstat[18])+stof(vmstat[19])) ,stof(vmstat[16])  , stof(vmstat[17])  ,stof(vmstat[18])  ,stof(vmstat[19])  ,100*stof(vmstat[27])/(0.1+stof(vmstat[27])+stof(vmstat[28])),stof(vmstat[27])  ,stof(vmstat[28])  ,stof(vmstat[8])  ,stof(vmstat[9])  ,int(vmstat[10]) , int(vmstat[11]) ,int(vmstat[12]) ,float(vmstat[13]) ,float(vmstat[14]) ,float(vmstat[15]) ,stof(vmstat[29])  , stof(vmstat[30])  ,float(vmstat[25]) , float(vmstat[26]) , float(vmstat[22]) ,float(vmstat[23]) ,float(vmstat[24]) ,ps_root  ,ps_other ,module_num[0] ,module_num[1] ,module_num[2] ,module_num[3] ,use_cpu_out ,read_disk_out , write_disk_out ,recv_net_out ,recv_netp_out ,send_net_out ,send_netp_out , lsmod_out ,ps_out ,0)
                #print "5553"
                state_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+vmname+'.statedetect', 'r')
                state_Det = state_detect.readline()
                state_detect.close()
                #print "5554"
                if int(state_Det)==0:
                    printlog("wait stop exdamain!")
                    '''
                    time.sleep(6)
                    state='m'
                    while state=='m':
                        state_d = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+vmname+'.state', 'r')
                        state = state_d.readline()
                        state_d.close()
                        time.sleep(2)
                    printlog("stop exdamain!")
                    '''
                    print "-777"
                    break
                else:
                    print "777"
                    data = db.oncesql(sql)
                    printlog("add 1 to "+tablename)
                    
            except IndexError:
                print "list out range"
                continue
        #stopServer()
    except Exception:
        print "exdamain fail"
        printlog("exdamain fail ")

if __name__ =='__main__':
    exdamain(sys.argv[1],sys.argv[2])
    '''
    try:
        signal.signal(signal.SIGINT,quit)
        signal.signal(signal.SIGTERM,quit) 
        exdamain(sys.argv[1],sys.argv[2])
    except Exception:
        print "over"
    '''
