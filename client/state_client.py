#!/usr/bin/python
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
import sys
import socket 
import time 
import commands
def ps_stat():
    #output = commands.getstatusoutput('ps -aux')
    ps_res = subprocess.Popen(['ps','axco','user,pid,time,%cpu,%mem,command'], stdout=subprocess.PIPE)
    return ps_res
    
def module_stat():
    #output = commands.getstatusoutput('ps -aux')
    lsmod_res = subprocess.Popen(['lsmod'], stdout=subprocess.PIPE)
    return lsmod_res    
 
if __name__=='__main__':
    HOST = '223.3.85.28'  
    PORT = 9999  
    
    s = socket.socket()
    while True:
        try:
            #s.settimeout(1)
            s.connect((HOST,PORT))
            print "connect success!"
            #get dstat value
            stat_popen = subprocess.Popen(['dstat','-cdgilmnprsy','--noheaders'],stdout = subprocess.PIPE)
            break
        except:
            print "connect fail, again"
            time.sleep(2)
    a=0
    dstate = 1
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
        try:
            s.sendall('s\n'+dstat)  
            s.sendall('p\n'+ps_stat().stdout.read()+str(time.time())+' p p p p p')
            s.sendall('m\n'+module_stat().stdout.read())      
            #sys.stdout.flush()
            print s.recv(1024)
        #time.sleep(1) 
        #if a == 10:  
            #break 
        except:
            stat_popen.kill()
            s.close()
            s = socket.socket()
            while True:
                try:
                #s.settimeout(1)
                    s.connect((HOST,PORT))
                    print "connect success!, again"
                    stat_popen = subprocess.Popen(['dstat','-cdgilmnprsy','--noheaders'],stdout = subprocess.PIPE)
                    #get dstat value
                    break
                except:
                    print "lost connect, again"
                    time.sleep(2)

    s.close()