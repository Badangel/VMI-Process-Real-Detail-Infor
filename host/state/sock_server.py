import SocketServer  
import time  
import signal  
import global_var as Globalvar
from vmaddr_config import vmaddr
from warning_log import *
class MyHandler(SocketServer.BaseRequestHandler):  
    def handle(self):
        try:
            b=True
            a=0  
            conn = self.request
            data = conn.recv(2048)
            data = conn.recv(2048)
            vmname = vmaddr[self.client_address[0]]
            printlog('socket vm is: '+vmname)

            while True and Globalvar.getexit(): 
                a=a+1
                #print ('wait....',a)
                data = conn.recv(18900)
                #print len(data),data,time.ctime()
                #print ('data:',data)
                if data.strip():
                    stat = data[0]
                    if stat=='s':
                        d_stat=data[2:]
                        file_object = open('tempfile/'+vmname+'.d_stat','w')
                        file_object.writelines(d_stat)
                        #print 'write to d_stat',vmname
                        #print self.client_address,time.ctime(),' dstat\n',d_stat
                    if stat=='p':
                        ps_stat=data[2:]
                        file_ps = open('tempfile/'+vmname+'.ps','w')
                        #print 'write to ps',vmname
                        file_ps.writelines(ps_stat)
                        #print self.client_address,time.ctime(),' ps\n',ps_stat
                    if stat=='m':
                        lsmod_stat=data[2:]
                        file_module = open('tempfile/'+vmname+'.module','w')
                        file_module.writelines(lsmod_stat)
                        #print 'write to module',vmname
                        #print self.client_address,time.ctime(),' module\n',lsmod_stat
                    
                '''    
                print stat,'\n\n\n\n\n\n'
                print ('ps wait....')
                ps_data = self.request.recv(8900)  
                print ps_data,time.ctime(),self.client_address
                print ('lsmod wait....')
                lsmod_data = self.request.recv(5000)  
                print lsmod_data,'\n over!!',time.ctime(),self.client_address  
                ''' 
            # kel = raw_input('>>>')
                #   if data == 'exit': 
                #if kel == 'exit': 
                conn.send( ' %s %s ' % (stat,time.ctime())) 
        except Exception:
            print "port error"
                 

class ServerWork(object):
    def __init__(self, host, port):
        self.host = host
        self.port =port
        self.server=SocketServer.ThreadingTCPServer((self.host,self.port),MyHandler)

    def server_start(self):
        print "server start!!!"        
        self.server.serve_forever()
        
    def server_stop(self):
        self.server.shutdown()
        self.server.server_close()
