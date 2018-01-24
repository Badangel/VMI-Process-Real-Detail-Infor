import time
import pyvmi

def warining_w(vmi, pid, pname):
    file_warning = open('warning.log', 'a')
    file_warning.write(str(time.ctime())+' warning :'+str(pid)+' '+str(pname)+'is hided!\n')
    file_warning.close()