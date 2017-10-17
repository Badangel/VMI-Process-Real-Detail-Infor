
''' 
def stat():   
    stat = {}  
    f = open("/proc/stat")  
    lines = f.readlines()  
    f.close()
    indexx=0;  
    for line in lines:  
        indexx=indexx+1  
        linecpu= line
        if indexx==1:
            break  
    return linecpu
'''
import subprocess
#get dstat value
stat_popen = subprocess.Popen(['dstat','-cdgilmnprsy','--noheaders'], stdout = subprocess.PIPE)

import commands
def ps_stat():
    #output = commands.getstatusoutput('ps -aux')
    ps_res = subprocess.Popen(['ps','axo','user,pid,time,%cpu,%mem'], stdout=subprocess.PIPE)
    return ps_res
    
def module_stat():
    #output = commands.getstatusoutput('ps -aux')
    lsmod_res = subprocess.Popen(['lsmod'], stdout=subprocess.PIPE)
    return lsmod_res    
 
import socket 
import time 

HOST = '223.3.85.28'  
PORT = 9999  
  
s = socket.socket()
while True:
    try:
        s.settimeout(1)
        s.connect((HOST,PORT))
        print "connect success!"
        break
    except:
        print "connect fail, again"
        time.sleep(2)
a=0; 
#si=len(stat_popen.stdout.readline())
#print ('changdu',si)

while True:  
    #kel = raw_input('>>>') 
    a=a+1
   # print stat() 
    dstat='u'
    while dstat[0] != ' ':
        dstat=stat_popen.stdout.readline()
     #   print dstat[0]
   # print dstat
    s.sendall('s\n'+dstat)  
    s.sendall('p\n'+ps_stat().stdout.read())
    s.sendall('m\n'+module_stat().stdout.read())
    print s.recv(1024) 
    #time.sleep(1) 
    if a == 10:  
        break 

stat_popen.kill()
s.close()