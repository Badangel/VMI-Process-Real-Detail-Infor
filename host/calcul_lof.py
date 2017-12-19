from state.DBHelper import DBHelper
import math
import Queue
import time,threading
from state.external_data import exdamain
from state.external_data import startServer

#Calculate the distance of each point
def getdisance(data,datalen):
    #print data
    dismatrix = [[0 for i in range(datalen)] for i in range(datalen)]
    
    for i in range(0,datalen-1):
        for j in range(i+1,datalen):
            dis = 0
            for l in range(1,len(data[0])):
                dis = dis + (data[i][l]-data[j][l])*(data[i][l]-data[j][l])
            dis = math.sqrt(dis)
            dismatrix[i][j] = dis
            dismatrix[j][i] = dis
    return dismatrix


#Calculate the K distance of each point
def getk_distance(matrix,datalen,k):
    kdis = [0 for i in range(datalen)]
    for i in range(datalen):
        pq = Queue.PriorityQueue(k)
        for j in range(datalen):
            if i!=j:
                if pq.full() == False:
                    pq.put(matrix[i][j]*-1)
                else:
                    kdis[i] = pq.get()
                    if matrix[i][j]*-1>kdis[i]:
                        pq.put(matrix[i][j]*-1)
                    else:
                        pq.put(kdis[i])
        kdis[i] = pq.get()*-1
    #print("kids:",kdis)
    return kdis

#Calculate reachable disdance of each point
def getreach_distance(matrix,datalen,kdis):
    reachdis_matrix = [[0 for i in range(datalen)] for i in range(datalen)]
    for i in range(datalen):
        for j in range(datalen):
            if i==j:
                reachdis_matrix[i][j] = 0
            else:
                if matrix[i][j] > kdis[j]:
                    reachdis_matrix[i][j] = matrix[i][j]
                else:
                    reachdis_matrix[i][j] = kdis[j]
    #print("reachdis_matrix:"reachdis_matrix)
    return reachdis_matrix


#Calculate local reachable density of each point
def getlrd(reachdis_matrix,matrix,datalen,minpts):
    lrd = [0 for i in range(datalen)]
    for i in range(datalen):
        lrdpq = Queue.PriorityQueue(minpts)
        for j in range(datalen):
            if i!=j:
                if lrdpq.full() == False:
                    lrdpq.put([matrix[i][j]*-1,j])
                else:
                    temp = lrdpq.get()
                    if matrix[i][j]*-1 > temp[0]:
                        lrdpq.put([matrix[i][j]*-1,j])
                    else:
                        lrdpq.put(temp)
        while not lrdpq.empty():
            temp = lrdpq.get()
            lrd[i] = lrd[i] + reachdis_matrix[i][temp[1]]
        lrd[i] = minpts/(lrd[i]+1)

    #print("lrd:",lrd)
    return lrd

                        
#Calculate LOF of each point
def getlof(data,k,minpts,datalen,dismatrix,lrd):
    '''
    datalen = len(data)
    dismatrix = getdisance(data,datalen)
    kdis = getk_distance(dismatrix,datalen,k)
    reach_mat = getreach_distance(dismatrix,datalen,kdis)
    lrd = getlrd(reach_mat,dismatrix,datalen,minpts)
    '''

    lof = [0 for i in range(datalen)]
    for i in range(datalen):
        lofpq = Queue.PriorityQueue(minpts)
        for j in range(datalen):
            if i!=j:
                if lofpq.full() == False:
                    lofpq.put([dismatrix[i][j]*-1,j])
                else:
                    temp = lofpq.get()
                    if dismatrix[i][j]*-1 > temp[0]:
                        lofpq.put([dismatrix[i][j]*-1,j])
                    else:
                        lofpq.put(temp)
        while not lofpq.empty():
            temp = lofpq.get()
            lof[i] = lof[i] + lrd[temp[1]]
        lof[i] = (lof[i]/minpts)/lrd[i]
    return lof

#Calculate LOF of one point
def getoplof(data,k,minpts,datalen,kdis,lrd,onepoint):
    oplofdis = [0 for i in range(datalen)]
    for i in range(datalen):
        for j in range(1,len(data[0])):
            oplofdis[i] = oplofdis[i] + (data[i][j]-onepoint[j])*(data[i][j]-onepoint[j])
        oplofdis[i] = math.sqrt(oplofdis[i])
    lofpq = Queue.PriorityQueue(minpts)
    for j in range(datalen):
        if lofpq.full() == False:
            lofpq.put([oplofdis[j]*-1,j])
        else:
            temp = lofpq.get()
            if oplofdis[j]*-1 > temp[0]:
                lofpq.put([oplofdis[j]*-1,j])
            else:
                lofpq.put(temp)
    opreadis = 1
    klrd = 0
    while not lofpq.empty():
        temp = lofpq.get()
        #print temp,lrd[temp[1]],' ',kdis[temp[1]],oplofdis[temp[1]]
        klrd = klrd + float(lrd[temp[1]])
        if float(kdis[temp[1]]) > oplofdis[temp[1]]:
            opreadis = opreadis + float(kdis[temp[1]])
        else:
            opreadis = opreadis + oplofdis[temp[1]]
    #oplrd = minpts/opreadis
    oplof = (opreadis*klrd)/(minpts*minpts) #(klrd/k)/oplrd
    return oplof

def modifypsstate1(dataid):
     changestate = "update psinfo set state = 1 where id =" + str(dataid)
     db.oncesql(changestate)

def modifypsstate0(dataid):
     changestate = "update psinfo set state = 0 where id =" + str(dataid)
     db.oncesql(changestate)
    
def modifypsstat(largeid):
    sqlps = "select id,prio,majflt,utime,stime,start_time,realstart_time,totalfiles,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,totalsyscall,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from psinfo where state = 1 and id < "+str(largeid)
    psdata = db.oncesql(sqlps)
    print "psdata len:",len(psdata)
    datalen = len(psdata)

    #calculate 2000 data offline and write to files
    dismatrix = getdisance(psdata,datalen)
    kdis = getk_distance(dismatrix,datalen,K)
    reach_mat = getreach_distance(dismatrix,datalen,kdis)
    lrd = getlrd(reach_mat,dismatrix,datalen,MinPts)
    pslof = getlof(psdata,K,MinPts,datalen,dismatrix,lrd)

    #delete minflt
    sqlpsone = "select id,prio,majflt,utime,stime,start_time,realstart_time,totalfiles,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,totalsyscall,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from psinfo where state = 0"
    psonedata = db.oncesql(sqlpsone)
    psonedata = list(psonedata)
    
    for a in range(0,len(psonedata)):
        alof = getoplof(psdata,K,MinPts,datalen,kdis,lrd,psonedata[a][1:])
        changestate = ""
        if alof > 1.1 or alof < 0.9:
            changestate = "update psinfo set state = 1 where id =" + str(psonedata[a][0])
            print "add in",
            db.myupdate(changestate)
            print psonedata[a][0],alof
        else:
            changestate = "update psinfo set state = 2 where id =" + str(psonedata[a][0])
            db.myupdate(changestate)
            print "move out",psonedata[a][0],alof
    db.allcommit()


def trainState(data,datalen,K,MinPts):
    dismatrix = getdisance(data,datalen)
    kdis = getk_distance(dismatrix,datalen,K)
    reach_mat = getreach_distance(dismatrix,datalen,kdis)
    lrd = getlrd(reach_mat,dismatrix,datalen,MinPts)
    print "train over!!"
    return kdis,lrd

def detectState():
    K = 5
    MinPts = 5
    selectdb = DBHelper()
    sqlstate = "select 100-idl_cpu_in,usep_mem_in,usep_swap_in,pagein_in,pageout_in,interrupts1_in,interrupts2_in,interrupts3_in,loadavg1_in*100,loadavg5_in*100,loadavg15_in*100,read_total_in,writ_total_in,ps_root_in,ps_other_in,use_cpu_out,recv_net_out,recv_netp_out,send_net_out,send_netp_out,lsmod_out,ps_out from state where stat = 1"
    statedata = selectdb.oncesql(sqlstate)
    datalen = len(statedata)
    kdis,lrd = trainState(statedata,datalen,K,MinPts)

    startServer()

    t1 = threading.Thread(target = exdamain,args =[],name='getstate')
    t1.setDaemon(True)
    t1.start()

    sqlstate1 = "select id,100-idl_cpu_in,usep_mem_in,usep_swap_in,pagein_in,pageout_in,interrupts1_in,interrupts2_in,interrupts3_in,loadavg1_in*100,loadavg5_in*100,loadavg15_in*100,read_total_in,writ_total_in,ps_root_in,ps_other_in,use_cpu_out,recv_net_out,recv_netp_out,send_net_out,send_netp_out,lsmod_out,ps_out from state where stat = 0"
    i = 0
    while True:
        i = i + 1
        del selectdb
        selectdb = DBHelper()
        statedatanew = selectdb.oncesql(sqlstate1)
        statedatanew = list(statedatanew)
        datanewlen = len(statedatanew)
        if datanewlen == 0:
            time.sleep(1)
            print "sleep 1!!",i
            continue
        else:
            print 'start time:',time.clock()
            addnum = 0
            for a in range(0,datanewlen):
                alof = getoplof(statedata,K,MinPts,datalen,kdis,lrd,statedatanew[a][1:])
                changestate = ""
                if alof > 1.1 or alof < 0.9:
                    changestate = "update state set stat = 1 where id =" + str(statedatanew[a][0])
                    addnum = addnum + 1
                    print "add in",
                    selectdb.myupdate(changestate)
                    #db.oncesql(changestate)
                    print statedatanew[a][0],alof,time.clock()
                
                else:
                    changestate = "update state set stat = 2 where id =" + str(statedatanew[a][0])
                    selectdb.oncesql(changestate)
                    print "move out",statedatanew[a][0],alof,time.clock()
            selectdb.allcommit()


def detectPsinfo():
    K = 5
    MinPts = 5
    db = DBHelper()
    '''
    sqlstate = "select 100-idl_cpu_in,usep_mem_in,usep_swap_in,pagein_in,pageout_in,interrupts1_in,interrupts2_in,interrupts3_in,loadavg1_in*100,loadavg5_in*100,loadavg15_in*100,read_total_in,writ_total_in,ps_root_in,ps_other_in,use_cpu_out,recv_net_out,recv_netp_out,send_net_out,send_netp_out,lsmod_out,ps_out from state"
    data = db.oncesql(sqlstate)
    datalen = len(data) 
    print "len: ",datalen,len(data[0])
    print len(getlof(data,K,MinPts))
    '''
    
    #delete minflt
    sqlps = "select id,prio,majflt,utime,stime,start_time,realstart_time,totalfiles,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,totalsyscall,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from psinfo where state = 1"
    psdata = db.oncesql(sqlps)
    print "psdata len:",len(psdata)
    datalen = len(psdata)
  

    #calculate 2000 data offline and write to files
    dismatrix = getdisance(psdata,datalen)
    kdis = getk_distance(dismatrix,datalen,K)
    reach_mat = getreach_distance(dismatrix,datalen,kdis)
    lrd = getlrd(reach_mat,dismatrix,datalen,MinPts)
    pslof = getlof(psdata,K,MinPts,datalen,dismatrix,lrd)
    
    filekdis = open ('kdisfile.py', 'w') 
    filekdis.writelines('kdis = '+str(kdis))
    filelrd = open ('lrdfile.py', 'w')
    filelrd.writelines('lrd = '+str(lrd))
    fileHandle = open ('test.txt', 'w') 
    for i in range(len(pslof)):
        temp = str(psdata[i][0])+" "+str(pslof[i])+"\n"
        fileHandle.write(temp)
        #print temp
    fileHandle.flush()
    '''
    #read offline data
    from kdisfile import kdis
    from lrdfile import lrd
    '''
    '''
    #delete minflt
    sqlpsone = "select id,prio,majflt,utime,stime,start_time,realstart_time,totalfiles,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,totalsyscall,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from psinfo where id > 8000 and id < 13001"
    psonedata = db.oncesql(sqlpsone)
    psonedata = list(psonedata)
    
    print "6138 select ok"
    print time.clock()
    addnum = 0
    for a in range(0,len(psonedata)):
        alof = getoplof(psdata,K,MinPts,kdis,lrd,psonedata[a][1:])
        changestate = ""
        if alof > 1.1 or alof < 0.9:
            changestate = "update psinfo set state = 1 where id =" + str(psonedata[a][0])
            addnum = addnum + 1
            print "add in",
            db.myupdate(changestate)
            #db.oncesql(changestate)
            print psonedata[a][0],alof,time.clock()
        
        else:
            changestate = "update psinfo set state = 2 where id =" + str(psonedata[a][0])
            db.oncesql(changestate)
            print "move out",psonedata[a][0],alof,time.clock()

    db.allcommit()
    print 'add:',addnum
    '''

if __name__ =='__main__':
    detectState()
    