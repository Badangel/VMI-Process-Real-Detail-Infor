import Queue
import math
import DBHelper


#Calculate the distance of each point
def getdisance(data,datalen):
    #print data
    dismatrix = [[0 for i in range(datalen)] for i in range(datalen)]
    
    for i in range(datalen-1):
        for j in range(i+1,datalen):
            dis = 0
            for l in range(len(data[0])):
                #print("i:",i," j:",j," k:",k)
                dis = dis + (data[i][l]-data[j][l])*(data[i][l]-data[j][l])
            dis = math.sqrt(dis)
            dismatrix[i][j] = dis
            dismatrix[j][i] = dis
    #for d in range(datalen):
    #    print dismatrix[d]
    return dismatrix

#Calculate the Knn distance of each point
def getknn(matrix,datalen,k):
    kdis = [0 for i in range(datalen)]
    for i in range(datalen):
        pq = Queue.PriorityQueue(k)
        for j in range(datalen):
            if i!=j:
                if pq.full() == False:
                    pq.put([matrix[i][j]*-1,j])
                else:
                    temp = pq.get()
                    if matrix[i][j] < temp[0]*-1:
                        pq.put([matrix[i][j]*-1,j])
                    else:
                        pq.put(temp)
        while not pq.empty():
            temp = pq.get()
            kdis[i] = kdis[i] + temp[0]*-1
            #print temp,
        kdis[i] = kdis[i]/k
        #print " "
    #print("kids:",kdis)
    return kdis

#Calculate the Knn proportion with k point of each point
def getknnpro(data,k):
    datalen = len(data)
    knndismatrix = getdisance(data,datalen) 
    kdis = getknn(knndismatrix,datalen,k)
    knnpro = [0 for i in range(datalen)]
    for i in range(datalen):
        pq = Queue.PriorityQueue(k)
        for j in range(datalen):
            if i!=j:
                if pq.full() == False:
                    pq.put([knndismatrix[i][j]*-1,j])
                else:
                    temp = pq.get()
                    if knndismatrix[i][j] < temp[0]*-1:
                        pq.put([knndismatrix[i][j]*-1,j])
                    else:
                        pq.put(temp)
        while not pq.empty():
            temp = pq.get()
            knnpro[i] = knnpro[i] + kdis[temp[1]]
            #print temp,
        knnpro[i] = k*kdis[i]/(knnpro[i]+1)
        #print " "
    #print("kids:",kdis)
    return knnpro

#Calculate knn proportion of one point
def getopknn(data,kdis,datalen,k,onepoint):
    print onepoint
    opknndis = [0 for i in range(datalen)]
    for i in range(datalen):
        for j in range(len(data[0])):
            opknndis[i] = opknndis[i] + (data[i][j]-onepoint[j])*(data[i][j]-onepoint[j])
        opknndis[i] = math.sqrt(opknndis[i])
    
    print "cal dis"#,opknndis
    pq = Queue.PriorityQueue(k)
    for i in range(datalen):
        if pq.full() == False:
            pq.put([opknndis[i]*-1,i])
        else:
            temp = pq.get()
            if opknndis[i] < temp[0]*-1:
                pq.put([opknndis[i]*-1,i])
            else:
                pq.put(temp)
    print "cal k point"
    kdisp = 0
    kdisk = 1
    while not pq.empty():
        temp = pq.get()
        kdisp = kdisp + temp[0]*-1
        kdisk = kdisk + kdis[temp[1]]
        print temp[0],kdis[temp[1]],temp[1]
    opknn = kdisp/kdisk
    return opknn





if __name__ == '__main__':
    '''test data
    data = [[55,88,40,62,75],
            [40,85,45,65,75],
            [55,80,40,58,75],
            [45,88,42,62,70],
            [55,82,38,66,78],
            [55,50,99,99,20],
            [58,70,40,60,80],
            [0,0,0,0,0]] 
    '''

    K = 4
    db = DBHelper.DBHelper()
    
    sqlps = "select prio,minflt,majflt,utime,stime,start_time,realstart_time,totalfiles,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,totalsyscall,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from psinfo limit 0,1500"
    psdata = db.oncesql(sqlps)

    #knnp = getknnpro(psdata,K)

    datalen = len(psdata)
    knndismatrix = getdisance(psdata,datalen) 
    kdis = getknn(knndismatrix,datalen,K)
    print "kdis ok"
    
    sqlpsone = "select prio,minflt,majflt,utime,stime,start_time,realstart_time,totalfiles,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,totalsyscall,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from psinfo where id = 5949"
    psonedata = db.oncesql(sqlpsone)
    print "6138 select ok"
    print getopknn(psdata,kdis,datalen,K,psonedata[0])

    sqlpsone = "select prio,minflt,majflt,utime,stime,start_time,realstart_time,totalfiles,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,totalsyscall,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from psinfo where id = 6265"
    psonedata = db.oncesql(sqlpsone)
    print "6265 select ok"
    print getopknn(psdata,kdis,datalen,K,psonedata[0])
    #print knnp[1400:1450]

  
