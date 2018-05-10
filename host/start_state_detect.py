#!/usr/bin/python
import threading,time,sys
import state.external_data as exdata
from calcul_lof import detectState

from state.warning_log import *

def start_state(domname):
    t0 = threading.Thread(target = detectState,args =[domname,'nowstate'],name='detectstate')
    t0.setDaemon(False)
    t0.start()
    	    
    printlog("will start exdamain")
    t1 = threading.Thread(target = exdata.exdamain,args =[domname,'nowstate'],name='getstate')
    t1.setDaemon(False)
    t1.start()
    printlog("start over")


if __name__=='__main__':
    domname = sys.argv[1]
    print 'enter detect domU state'
    state_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.statedetect', 'r')
    state_Det = state_detect.readline()
    state_detect.close()
    print int(state_Det)
    if int(state_Det)==0:
        state_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.statedetect', 'w')
        state_detect.write("1")
        state_detect.close()
        start_state(domname)
        

        #print 'start over'
        #state_Det = 1
        #while state_Det>0:
            #time.sleep(1)
            #state_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.statedetect', 'r')
            #state_Det = int(state_detect.readline())
            #state_detect.close()
            #printlog("main run!!")
    
