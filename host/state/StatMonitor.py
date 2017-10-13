import subprocess
import time
import commands
import sys
import os
import re



while True:
    print '+'
        #statmonitor_popen = subprocess.Popen(['xentop','-i 1','|awk\'{print $65,$66}\''], stdout = subprocess.PIPE)
        #statmonitor_popen.stdout.flush()
    statmonitor_popen = subprocess.Popen(['xentop','-i 2','-b'], stdout = subprocess.PIPE)
    outdstat = statmonitor_popen.stdout.readlines()
    outdstat = statmonitor_popen.stdout.readlines()
    vmoutstat = re.compile('\s([0-9.]+?)\s').findall(outdstat[2])
    time.sleep(1)
    print vmoutstat
  
