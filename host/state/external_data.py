from server import *
import SocketServer
import time,threading
import DBHelper
import re

def stof(num):
    if num[-1] == "K":
        res = int(re.sub("\D","",num))
        return res
    else:
        if num[-1] == 'M':
            res = int(re.sub("\D","",num))*1024
            return res
        else:
            res = int(re.sub("\D","",num))/1024.0
            return res

def Noequalzero(vmnewstat,vmoldstat)
    n = len(vmnewstat)
    res = vmoldstat
    for i in range(0,n):
        if vmnewstat[i]!= '0':
            res[i] = vmnewstat[i]
    return res

ser=serverwork('223.3.85.28',9999)
##k=thread.start_new_thread(server_start(),ser)
t = threading.Thread(target = ser.server_start,args =[],name='test')
t.setDaemon(True)
t.start()
i = 0

db = DBHelper.DBHelper()

while (i<10):
    print i
    time.sleep(1)
    file_dstat = open('d_stat', 'r+')
    dstat_lines = file_dstat.readline( )
    if len(dstat_lines) == 0:
        print 'No data'
        continue
    #print re.compile('\s[0-9a-zA-Z.]+?\s').findall(dstat_lines)
    k=dstat_lines.replace('|','  ')
    vmstat = re.compile('([0-9a-zA-Z.]+?)\s').findall(k)
    print vmstat
    file_dstat.truncate(0)
    
    file_module = open('module', 'r+')
    module_lines = file_module.readlines( )
    module0_num = 0
    module1_num = 0
    module2_num = 0
    moduleo_num = 0
    for a in module_lines:
        module_apply = re.findall('\s([0-9]{1,3})\s',a)
        #print module_apply
        if len(module_apply) == 0:
            continue
        if module_apply[0] == '0':
            module0_num+=1
        else:
            if module_apply[0] == '1':
                module1_num+=1
            else:
                if module_apply[0] == '2':
                    module2_num+=1
                else:
                    moduleo_num+=1
        
    print 'module num: ',len(module_lines)-1,' ',module0_num,' ',module1_num,' ',module2_num,' ',moduleo_num
    file_module.truncate(0)

    file_ps = open('ps', 'r+')
    ps_lines = file_ps.readlines()
    ps_root = 0
    ps_other = 0
    
    for a in ps_lines:
        if a[0:4]== 'USER':
            continue
        if a[0:4] == 'root':
            ps_root+=1
        else:
            ps_other+=1
    print 'ps num: ',len(ps_lines)-1,' ',ps_root,' ',ps_other
    file_ps.truncate(0)

    statmonitor_popen = subprocess.Popen(['xentop','-i 1','-b'], stdout = subprocess.PIPE)
    outdstat = statmonitor_popen.stdout.readlines()
    vmoutlaststat
    vmoutstat = re.compile('\s([0-9.]+?)\s').findall(outdstat[2])
    cpuoutuse = float(vmoutstat[1])*100
    vmoutlaststat = Noequalzero(vmoutstat,vmoutlaststat)
    i=i+1
   # sql = "insert into state(domid,domname,usr_cpu_in,sys_cpu_in ,idl_cpu_in,wai_cpu_in,hiq_cpu_in ,siq_cpu_in,read_disk_in , write_disk_in ,recv_net_in ,send_net_in ,usd_mem_in , buff_mem_in ,cach_mem_in ,free_mem_in ,usd_swap_in ,free_swap_in ,pagein_in ,pageout_in ,interrupts1_in , interrupts2_in ,interrupts3_in ,loadavg1_in ,loadavg5_in ,loadavg15_in ,int_sys_in , csw_sys_in ,read_total_in , writ_total_in , run_procs_in ,blk_procs_in ,new_procs_in ,ps_root_in , ps_vm_in ,ps_other_in ,lsmod0_in ,lsmod1_in ,lsmod2_in ,lsmodother_in ,use_cpu_out ,read_disk_out , write_disk_out ,recv_net_out ,send_net_out , lsmod_out ,ps_out ) values('%d','%s','%d','%d' ,'%d','%d','%d' ,'%d','%f' , '%f' ,'%f' ,'%f' ,'%f' , '%f' ,'%f' ,'%f' ,'%f' ,'%f' ,'%f' ,'%f' ,'%d' , '%d' ,'%d' ,'%f' ,'%f' ,'%f' ,'%f' , '%f' ,'%f' , '%f' , '%f' ,'%f' ,'%f' ,'%d' , '%d' ,'%d' ,'%d' ,'%d' ,'%d' ,'%d' ,'%d' ,'%f' , '%f' ,'%f' ,'%f' , '%d' ,'%d')"%(0,'ubuntu1604',vmstat[0],vmstat[1] ,vmstat[2],vmstat[3],vmstat[4] ,vmstat[5],stof(vmstat[6]) , stof(vmstat[7])  ,stof(vmstat[20])  ,stof(vmstat[21])  ,stof(vmstat[16])  , stof(vmstat[17])  ,stof(vmstat[18])  ,stof(vmstat[19])  ,stof(vmstat[27])  ,stof(vmstat[28])  ,stof(vmstat[8])  ,stof(vmstat[9])  ,vmstat[10] , vmstat[11] ,vmstat[12] ,vmstat[13] ,vmstat[14] ,vmstat[15] ,stof(vmstat[29])  , stof(vmstat[30])  ,vmstat[25] , vmstat[26] , vmstat[22] ,vmstat[23] ,vmstat[24] ,ps_root_in , ps_vm_in ,ps_other_in ,lsmod0_in ,lsmod1_in ,lsmod2_in ,lsmodother_in ,use_cpu_out ,read_disk_out , write_disk_out ,recv_net_out ,send_net_out , lsmod_out ,ps_out )
   # data = db.oncesql(sql)
print ('?????')
time.sleep(12)
