import subprocess
import time
import commands
import sys
import os
import re




print '+'
    #statmonitor_popen = subprocess.Popen(['xentop','-i 1','|awk\'{print $65,$66}\''], stdout = subprocess.PIPE)
    #statmonitor_popen.stdout.flush()
statmonitor_popen = subprocess.Popen(['xentop','-i 1','-b'], stdout = subprocess.PIPE)
outdstat = statmonitor_popen.stdout.readlines()
vmoutstat = re.compile('\s([0-9.]+?)\s').findall(outdstat[2])
print vmoutstat
  
