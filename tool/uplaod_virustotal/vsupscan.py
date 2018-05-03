
from virustotal import vt
import time
vt = vt()
vt.setkey('081d04774806f1874c92ec5f886ea263e58fa636120624ecc31938fae109293d')
vt.out('print')

vslist = []
vslistfile = open('/home/vmimove/Downloads/code/VmiXen/tool/vsup/samplebinlist', 'r+')
vslistname = vslistfile.readlines()
for a in vslistname:
    a1 = a[:-2]
    vslist.append(a1)
vslistfile.close()
software = ['ClamAV','Kaspersky','McAfee','NANO-Antivirus','Qihoo-360','Rising','ESET-NOD32','Avast']
vsrdi = '/home/vmimove/Downloads/vsup/'
recorddir = '/home/vmimove/Downloads/code/VmiXen/tool/vsup/vsrecord'
n = 0
while n < 2:
    
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
