from server import *
import SocketServer
import time,threading
  
ser=serverwork('223.3.80.145',9999)
##k=thread.start_new_thread(server_start(),ser)
t = threading.Thread(target = ser.server_start,args =[],name='test')
t.setDaemon(True)
t.start()
for i in range(0,9):
    file_dstat = open('d_stat', 'r')
    dstat_lines = file_dstat.readline( )
    import re
    #print re.compile('\s[0-9a-zA-Z.]+?\s').findall(dstat_lines)
    k=dstat_lines.replace('|','  ')
    vmstat = re.compile('([0-9a-zA-Z.]+?)\s').findall(k)
    print vmstat
    
    file_module = open('module', 'r')
    module_lines = file_module.readlines( )
    print 'module num: ',len(module_lines)-1

    file_ps = open('ps', 'r')
    ps_lines = file_ps.readlines( )
    print 'ps num: ',len(ps_lines)-1
    time.sleep(1)

print ('?????')
time.sleep(12)
