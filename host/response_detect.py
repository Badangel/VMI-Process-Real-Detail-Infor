#!/usr/bin/python
import sys,time
import threading
import subprocess
import pexpect
from state.DBHelper import DBHelper

from state.warning_log import *

def pausevm(domname):
    printlog("pause vm")
    print "pause vm",domname
    printlog("pause over")
    pause_vm = subprocess.Popen(['xl','pause',domname], stdout = subprocess.PIPE)
    pause_vm.wait()
    #p_vm = pexpect.spawn('xl pause %s'%(domname))
    print 'pause over1'
    printlog("pause over2")

def closevm(domname):
    close_vm = subprocess.Popen(['xl','shutdown',domname], stdout = subprocess.PIPE)
    close_vm.wait()

def migratevm(domname):
    fout = open('/home/vmi/Downloads/code/VmiXen/host/log/migrate.log','a')

    printlog("migrate vm")
    selectdb = DBHelper.DBHelper()
    sqlstate = "select ip from iptable where factor = (select min(factor) from iptable)" 
    ipdata = selectdb.oncesql(sqlstate)
    ipdata1 = list(ipdata)
    ip = ipdata1[0][0]
    mypassword = '309911'
    printlog("migrate to "+str(ip))
    migrate_vm = pexpect.spawn('sudo /usr/sbin/xl migrate %s %s'%(domname,ip))
    migrate_vm.logfile = fout
    migrate_vm.expect('root@%s\'s password:'%(ip))
    printlog("migrate password")
    migrate_vm.sendline (mypassword)

    migrate_vm.expect(pexpect.EOF)
    fout.close()
    printlog("migrate password over")
    
    print 'migrate over!'
    printlog("migrate over2")
    state = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.state', 'w')
    state.write("mm")
    state.close()
    #migrate_vm = subprocess.Popen(['xl','migrate',domname,ip[0]], stdout = subprocess.PIPE)
    #migrate_vm.wait()

def responsevm(domname,way):
    state_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.statedetect', 'w')
    state_detect.write("0")
    state_detect.close()
    ps_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.psdetect', 'w')
    ps_detect.write("0")
    ps_detect.close()
    net_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.netdetect', 'w')
    net_detect.write("0")
    net_detect.close()
    printlog("reponse "+str(way))
    time.sleep(2)
    if way == 1:
        #pause_vm = subprocess.Popen(['xl','pause',domname], stdout = subprocess.PIPE)
        state = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.state', 'w')
        state.write("p")
        state.close()
        pausevm(domname)
    if way == 2:
        state = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.state', 'w')
        state.write("c")
        state.close()
        closevm(domname)
    if way == 3:
        state = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.state', 'w')
        state.write("m")
        state.close()
        migratevm(domname)

if __name__=="__main__":
    domname = sys.argv[1]
    way = int(sys.argv[2])
    t0 = threading.Thread(target = responsevm,args =[domname,way],name='responsevm')
    t0.setDaemon(False)
    t0.start()
    #responsevm(domname,way)
