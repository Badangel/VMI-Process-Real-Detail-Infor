#!/usr/bin/python
import sys,re
from state.external_data import stof

def get_inter_cpumem(vmname):
    while True:
        file_dstat = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+vmname+'.d_stat', 'r+')
        dstat_lines = file_dstat.readline()
        if len(dstat_lines) == 0:
            print 'No data'
            continue
        else:
            break
    #print re.compile('\s[0-9a-zA-Z.]+?\s').findall(dstat_lines)
    k=dstat_lines.replace('|','  ')
    vmstat = re.compile('([0-9a-zA-Z.]+?)\s').findall(k)
    memp = 100*stof(vmstat[16])/(stof(vmstat[16])+stof(vmstat[17])+stof(vmstat[18])+stof(vmstat[19]))
    print "<br>CPU:",100-int(vmstat[2]),"%"
    print "MEM:",int(memp),"%"
    print "<br>Disk Read:",vmstat[6],"Write:",vmstat[7]
    print "<br>Net Down:",vmstat[20],"Up:",vmstat[21]
    return 100-int(vmstat[2]),memp

if __name__ =='__main__':
    get_inter_cpumem(sys.argv[1])
