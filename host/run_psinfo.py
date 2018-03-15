import subprocess
import sys
import state.global_var as Globalvar
from state.warning_log import *

def runpsinfo(domname,sqltable):
    psmonitor_popen = subprocess.Popen(['/home/vmi/Downloads/code/VmiXen/host/processinfo/vmiprocessinfo',domname,sqltable], stdout = subprocess.PIPE)
    while True and Globalvar.getexit():
        line = psmonitor_popen.stdout.readline().strip()
        if len(line)==0:
            i =0
        else:
            cprintlog('psinfo print:'+line)
    psmonitor_popen.kill()
    psmonitor_popen.terminate()
    cprintlog('runpsinfo over!')

def runpsinfo_inter(domname,sqltable):
    psmonitor_popen = subprocess.Popen(['/home/vmi/Downloads/code/VmiXen/host/processinfo/vmiprocessinfo',domname,sqltable], stdout = sys.stdout)

    while True and Globalvar.getexit():
         a=0
    psmonitor_popen.terminate()
    printlog('runpsinfo over!')

if __name__ =='__main__':
    runpsinfo(sys.argv[1],sys.argv[2])