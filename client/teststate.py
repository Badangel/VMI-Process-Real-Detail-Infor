#!/usr/bin/python
import time,sys,os,threading,subprocess
e = True
ed = True
def usecpu():  
    while True and e:  
         pass  
    print "usecpu over!"
         
def usemem():
    a = 8
    l = []
    while a>0:
        a=a-1
        s = ' ' * (100 * 1024 * 1024)
        l.append(s)
        time.sleep(1)   
    time.sleep(10)
    print "usemem over!"

def forkps():
    n=7
    while n>0:
        n=n-1
        pid = os.fork()
        if pid == 0:
            print "f"
        else:
            print "c"
    time.sleep(15)
    print "forkps over!"

def usedisk():
    while True and e: 
       commadcopy = 'cp /home/ubuntu1604/Downloads/300.file /home/ubuntu1604/Downloads/300c.file'
       docom = subprocess.Popen(['cp /home/ubuntu1604/Downloads/300.file /home/ubuntu1604/Downloads/300c.file'],stdout=subprocess.PIPE,shell=True)
       docom.wait()
       print 'copy over!'
       docom = subprocess.Popen(['rm /home/ubuntu1604/Downloads/300c.file'],stdout=subprocess.PIPE,shell=True)
       docom.wait()
       print 'rm over!'

    
if __name__ =='__main__':
    a = sys.argv[1]
    if a=='1':
        t0 = threading.Thread(target = usecpu,args =[],name='usecpu')
        t0.setDaemon(True)
        t0.start()
        time.sleep(15)
        e = False
    if a=='2':
        usemem()
    if a=='3':
        forkps()
    if a=='4':
        t1 = threading.Thread(target = usedisk,args =[],name='usedisk')
        t1.setDaemon(True)
        t1.start()
        time.sleep(15)
        ed= False
        docom1 = subprocess.Popen(['rm /home/ubuntu1604/Downloads/300c.file'],stdout=subprocess.PIPE,shell=True)
        docom1.wait()
    print 'bug over!'
        
