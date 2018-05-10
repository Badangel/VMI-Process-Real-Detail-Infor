#!/usr/bin/python
import sys
if __name__=='__main__':
    domname = sys.argv[1]
    state_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.netdetect', 'w')
    state_detect.write("0")
    state_detect.close()
