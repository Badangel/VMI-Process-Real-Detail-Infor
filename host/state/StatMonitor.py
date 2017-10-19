import subprocess
import time
import commands
import sys
import os
import re


net1old = ['0','0']
net0old = ['0','0']
while True:
    print '+'
        #statmonitor_popen = subprocess.Popen(['xentop','-i 1','|awk\'{print $65,$66}\''], stdout = subprocess.PIPE)
        #statmonitor_popen.stdout.flush()
    statmonitor_popen = subprocess.Popen(['xentop','-i 1','-nb'], stdout = subprocess.PIPE)
    outdstat = statmonitor_popen.stdout.readlines()
    #print outdstat
    vmnowstat = re.compile('\s([0-9.]+?)\s').findall(outdstat[2])
    net0now = re.compile('\s([0-9.]+?)bytes\s').findall(outdstat[3])
    net1now = re.compile('\s([0-9.]+?)bytes\s').findall(outdstat[4])
    time.sleep(1)
    recv_net_out = (int(net0now[1])+int(net1now[1])-int(net1old[1])-int(net0old[1]))/1024.0
    send_net_out = (int(net0now[0])-int(net0old[0]))/1024.0
    net1old = net1now
    net0old = net0now
    print recv_net_out,send_net_out
  
