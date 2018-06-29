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
    print "<CPU>"+str(100-int(vmstat[2]))+"%</CPU>"
    print "<MEM>"+str(int(memp))+"%</MEM>"
    print "<Disk_Read>",stof(vmstat[6]),"</Disk_Read><Disk_Write>",stof(vmstat[7]),"</Disk_Write>"
    print "<Net_Down>",stof(vmstat[20]),"</Net_Down><Net_Up>",stof(vmstat[21]),"</Net_Up>"
    return 100-int(vmstat[2]),memp

if __name__ =='__main__':
    get_inter_cpumem(sys.argv[1])
