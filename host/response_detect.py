#!/usr/bin/python
import sys,time
import subprocess

from state.DBHelper import DBHelper

from state.warning_log import *

def pausevm(domname):
    printlog("pause vm")
    print "pause vm",domname
    printlog("pause over")

    printlog("pause over2")

def closevm(domname):
    close_vm = subprocess.Popen(['xl','shutdown',domname], stdout = subprocess.PIPE)
    close_vm.wait()

def migratevm(domname):
    selectdb = DBHelper.DBHelper()
    sqlstate = "select ip from iptable where factor = = (select min(factor) from iptable)" 
    ipdata = selectdb.oncesql(sqlstate)
    ipdata1 = list(ipdate)
    ip = ipdata1[0]
    migrate_vm = subprocess.Popen(['xl','migrate',domname,ip[0]], stdout = subprocess.PIPE)
    migrate_vm.wait()

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
    if way == 1:
        print "pause vm",domname
        pause_vm = subprocess.Popen(['xl','pause',domname], stdout = subprocess.PIPE)
    if way == 2:
        closevm(domname)
    if way == 3:
        migratevm(domname)

if __name__=="__main__":
    domname = sys.argv[1]
    way = int(sys.argv[2])
    responsevm(domname,way)
