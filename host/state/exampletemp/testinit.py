#!/usr/bin/env python
import pyvmi
def testinit(self):
    vmi = pyvmi.init_complete("ubuntu1604")
    print "111"
    #print vmi
    print "222"

if __name__ == '__main__':
    print "write"
    f = open('test','w')
    temp = 'this i11s new'
    f.write(temp)
    f.close()
