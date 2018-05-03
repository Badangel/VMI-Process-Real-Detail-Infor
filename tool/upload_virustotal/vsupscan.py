
from virustotal import vt
import time
vt = vt()
vt.setkey('<---API key--->')
vt.out('print')

vslist = []
vslistfile = open('/home/ubuntu1604/Downloads/malware/recordvslist', 'r+')
vslistname = vslistfile.readlines()
for a in vslistname:
    a1 = a[:-2]
    vslist.append(a1)
vslistfile.close()
software = ['ClamAV','Kaspersky','McAfee','NANO-Antivirus','Qihoo-360','Rising','ESET-NOD32','Avast']
vsrdi = '/home/ubuntu1604/Downloads/malware/vsup/'
recorddir = '/home/ubuntu1604/Downloads/malware/vsrecord'
n = 584
while n < 1300:
    result  = vt.getfilereport(vsrdi+vslist[n],software)

    recordfile = open(recorddir, 'a+')
    recordfile.write(vslist[n])
    for r in result:
        recordfile.write(','+r)
    recordfile.write('\n')
    recordfile.close()
    print n,'is ok!'
    n = n+1
    time.sleep(15)
print 'over!!!'
