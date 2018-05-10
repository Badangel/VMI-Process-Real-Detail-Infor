import subprocess
import sys,time
import state.global_var as Globalvar
from state.warning_log import *

def runpsinfo(domname,sqltable):

    cprintfile = open('/home/vmi/Downloads/code/VmiXen/host/log/cprint.log', 'w')
    cprintfile.close()

    ps_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.psdetect', 'r')
    ps_Det = int(ps_detect.readline())
    ps_detect.close()
    while ps_Det==1:
        time.sleep(1)
        ps_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.psdetect', 'r')
        ps_Det = int(ps_detect.readline())
        ps_detect.close()

    psmonitor_popen = subprocess.Popen(['/home/vmi/Downloads/code/VmiXen/host/processinfo/vmiprocessinfo',domname,sqltable], stdout = subprocess.PIPE)
    while ps_Det==2:
        time.sleep(1)
        ps_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.psdetect', 'r')
        ps_Det = int(ps_detect.readline())
        ps_detect.close()
        '''
        line = psmonitor_popen.stdout.readline().strip()
        if len(line)==0:
            i =0
        else:
            cprintlog('psinfo print:'+line)
        '''
    psmonitor_popen.kill()
    psmonitor_popen.terminate()
    cprintlog('runpsinfo over!')

def runpsinfo_inter(domname,sqltable):
    psmonitor_popen = subprocess.Popen(['/home/vmi/Downloads/code/VmiXen/host/processinfo/vmiprocessinfo',domname,sqltable], stdout = sys.stdout)

    while True:
         a=0
    psmonitor_popen.terminate()
    printlog('runpsinfo over!')

if __name__ =='__main__':
    runpsinfo_inter(sys.argv[1],sys.argv[2])