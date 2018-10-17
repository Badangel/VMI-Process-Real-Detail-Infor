import SocketServer
from state.sock_server import *
import threading,time
def startServer():
    ser=ServerWork('223.3.69.62',9999)
    ##k=thread.start_new_thread(server_start(),ser)
    t = threading.Thread(target = ser.server_start,args =[],name='test')
    t.setDaemon(True)
    t.start()
    while True:
        time.sleep(1)
    return ser

if __name__=='__main__':
    startServer()
