#!/usr/bin/python
import time,os
import SocketServer  
import threading
def mem_stat():
    try:
        f = open('/proc/meminfo', 'r')
        for line in f:
            if line.startswith('MemTotal:'):
                mem_total = int(line.split()[1])
            elif line.startswith('MemFree:'):
                mem_free = int(line.split()[1])
            elif line.startswith('Buffers:'):
                mem_buffer = int(line.split()[1])
            elif line.startswith('Cached:'):
                mem_cache = int(line.split()[1])
            elif line.startswith('SwapTotal:'):
                vmem_total = int(line.split()[1])
            elif line.startswith('SwapFree:'):
                vmem_free = int(line.split()[1])
            else:
                continue
        f.close()
    except:
        return None
    physical_percent = usage_percent(mem_total - (mem_free + mem_buffer + mem_cache), mem_total)
    #physical_percent = usage_percent(mem_use, mem_total)
    virtual_percent = 0
    if vmem_total > 0:
        virtual_percent = usage_percent((vmem_total - vmem_free), vmem_total)
    return physical_percent, virtual_percent

def usage_percent(use, total):
    try:
        ret = (float(use) / total) * 100
    except ZeroDivisionError:
        raise Exception("ERROR - zero division error")
    return ret

def cpu_rate():
    total_a,idle_a=cpu_r()
    time.sleep(1)
    total_b,idle_b=cpu_r()
    sys_idle = idle_b - idle_a
    sys_total = total_b - total_a
    sys_us = sys_total - sys_idle
    cpu_a = (float(sys_us)/sys_total)*100
    return cpu_a

def cpu_stat():
    f = open("/proc/stat","r")
    for f_line in f:
      break
    f.close()
    f_line = f_line.split(" ")
    f_line_a=[]
    for i in f_line:
      if i.isdigit():
        i=int(i)
        f_line_a.append(i)
    total = sum(f_line_a)
    idle = f_line_a[3]
    return total,idle

def net_stat():  
    net = []  
    f = open("/proc/net/dev")  
    lines = f.readlines()  
    f.close()  
    net_reci = 0
    net_send = 0
    for line in lines[2:]:  
        if line.startswith('  eno1:'):
            net_reci=net_reci+int(line.split()[1])
            net_send=net_send+int(line.split()[9])
            #print line.split()
        elif line.startswith('xenbr0:'):
            net_reci=net_reci+int(line.split()[1])
            net_send=net_send+int(line.split()[9])
            #print line.split()
        else:
            continue
        '''
        con = line.split()  
        
        intf = dict(  
            zip(  
                ( 'interface','ReceiveBytes','ReceivePackets',  
                  'ReceiveErrs','ReceiveDrop','ReceiveFifo',  
                  'ReceiveFrames','ReceiveCompressed','ReceiveMulticast',  
                  'TransmitBytes','TransmitPackets','TransmitErrs',  
                  'TransmitDrop', 'TransmitFifo','TransmitFrames',  
                  'TransmitCompressed','TransmitMulticast' ),  
                ( con[0].rstrip(":"),int(con[1]),int(con[2]),  
                  int(con[3]),int(con[4]),int(con[5]),  
                  int(con[6]),int(con[7]),int(con[8]),  
                  int(con[9]),int(con[10]),int(con[11]),  
                  int(con[12]),int(con[13]),int(con[14]),  
                  int(con[15]),int(con[16]), )  
            )  
        )  
  
        net.append(intf) 
        ''' 
    return net_reci/(1024.0*1024),net_send/(1024.0*1024)

def disk_stat():  
    import os 
    hd={}  
    disk = os.statvfs("/")  
    hd['available'] = disk.f_bsize * disk.f_bavail  
    hd['capacity'] = disk.f_bsize * disk.f_blocks  
    hd['used'] = disk.f_bsize * disk.f_bfree 
    disk_file = open('/proc/diskstats','r')
    diskiotime = 0
    for line in disk_file:
        if line.startswith('   8       '):
            diskiotime=diskiotime+int(line.split(' ')[22][:-1])
            #print line.split(' ')[22][:-1]
        else:
            continue
    disk_file.close()
    return diskiotime/1000.0,100-hd['available']*100.00/hd['capacity']

def get_host_stat():
    phy_mem,val_mem = mem_stat()
    cpu_total,cpu_idl = cpu_stat()
    net_reci,net_send = net_stat()
    diskio_time,disk_use = disk_stat()
    time1 = time.time()
    print time1
    time.sleep(1)
    while True:
        phy_mem_next,val_mem_next = mem_stat()
        cpu_total_next,cpu_idl_next = cpu_stat()
        net_reci_next,net_send_next = net_stat()
        diskio_time_next,disk_use_next = disk_stat()
        time2 = time.time()
        print time2
        cpu_use = 100.0*(1-((cpu_idl_next*1.0-cpu_idl)/(cpu_total_next-cpu_total)))
        net_reci_use = (net_reci_next-net_reci)
        net_send_use = (net_send_next-net_send)
        diskio_use = (diskio_time_next-diskio_time)*100.0/(time2-time1)
        host_stat = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/host.state','w')
        #stri = cpu_use+','+phy_mem_next+','+val_mem_next+','+net_reci_use+','+net_send_use+','+disk_use_next+','+diskio_use
        stri = '%f,%f,%f,%f,%f,%f,%f' %(cpu_use,phy_mem_next,val_mem_next,net_reci_use,net_send_use,disk_use_next,diskio_use)
        host_stat.write(stri)
        host_stat.close()
        print 'cpu:',cpu_use
        print 'mem:',phy_mem_next,' swap:',val_mem_next
        print 'net reci:',net_reci_use,' send',net_send_use
        print 'disk:',disk_use_next,' io:',diskio_use
        cpu_total = cpu_total_next
        cpu_idl = cpu_idl_next
        net_reci = net_reci_next
        net_send = net_send_next
        diskio_time = diskio_time_next
        time.sleep(1)

class MyHandler(SocketServer.BaseRequestHandler):  
    def handle(self):
        try:
            conn = self.request
            data = conn.recv(2048)
            print self.client_address[0],data
            if data=='q':
                host_stat = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/host.state','r')
                stat=host_stat.readlines()
                print stat[0]
                conn.send( '%s' % (stat)) 
                host_stat.close()
        except Exception:
            aa=0
            print "port error"

class ServerHost(object):
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

if __name__=='__main__':
    ser=ServerHost('223.3.69.62',9998)
    ##k=thread.start_new_thread(server_start(),ser)
    t = threading.Thread(target = ser.server_start,args =[],name='hostinfo')
    t.setDaemon(True)
    t.start()
    
    get_host_stat()