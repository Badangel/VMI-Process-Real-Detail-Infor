import subprocess
import sys

def runpsinfo(domname):
    psmonitor_popen = subprocess.Popen(['/home/vmi/Downloads/code/VmiXen/host/processinfo/vmiprocessinfo',domname], stdout = subprocess.PIPE)
    while True:
        line=psmonitor_popen.stdout.readline().strip()
        if len(line)==0:
            i=0
        else:
            print('psinfo print:'+line)
    print 'runpsinfo over!'

if __name__ =='__main__':
    runpsinfo(sys.argv[1])