import shutil 
import os,sys,signal
import os.path
import subprocess,time
rootdir = "/home/ubuntu1604/Downloads/malware/sample/"

def run_all_exe(rootDir): 
    i = 0
    pslist = []
    for lists in os.listdir(rootDir): 
        i = i+1
        pspath = rootdir+lists
        print '[',i,']',pspath
        p = subprocess.Popen(pspath,stdout=sys.stdout,shell=True)
        #p.wait()
        pslist.append(p)
        time.sleep(4)
	print 'kill ',p.pid
        #os.killpg(p.pid,signal.SIGUSR1)
        p.terminate()
        p.wait()
        os.remove(pspath)
    return pslist
        #p.wait()
        #if int(lists)<501:
        #    path = os.path.join(rootDir, lists) 
         #   if os.path.isdir(path):
         #       Test2(path)
if __name__ =='__main__':
    ps = run_all_exe(rootdir)
    #for a in ps:
    #    a.kill()
    print 'all over!!'
      
