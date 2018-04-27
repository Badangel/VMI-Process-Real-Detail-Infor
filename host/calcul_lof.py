
#!/usr/bin/python
from state.DBHelper import DBHelper
import math
import Queue
import time,threading
#from state.external_data import exdamain
from run_psinfo import runpsinfo
import SocketServer
import state.external_data as exdata
#from state.external_data import startServer
#from state.external_data import stopServer
import state.global_var as Globalvar
import sys
import signal
from state.warning_log import *
from sklearn import tree

from sklearn import linear_model
from sklearn import naive_bayes
from sklearn import neighbors
from sklearn import svm
from sklearn import ensemble
from sklearn.model_selection import cross_val_score
from sklearn import preprocessing
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
def getlof(data,k,minpts):
    datalen = len(data)
    print "inlen:",datalen
    dismatrix = getdisance(data,datalen)
    kdis = getk_distance(dismatrix,datalen,k)
    reach_mat = getreach_distance(dismatrix,datalen,kdis)
    lrd = getlrd(reach_mat,dismatrix,datalen,minpts)

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
        print i,"(",
        while not lofpq.empty():
            temp = lofpq.get()
            print temp[1],
            lof[i] = lof[i] + lrd[temp[1]]
        lof[i] = (lof[i]/minpts)/lrd[i]
        print ")"
    return lof

#Calculate LOF of one point
def getoplof(data,k,minpts,datalen,kdis,lrd,onepoint):
    oplofdis = [0 for i in range(datalen)]
    for i in range(datalen):
        for j in range(0,len(onepoint)):
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
        #print temp[1],
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
    db = DBHelper()
    sqlps = "select id,prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,stack_vm,totalfiles,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,totalsyscall,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from psinfo where state = 1 and id < "+str(largeid)
    psdata = db.oncesql(sqlps)
    print "psdata len:",len(psdata)
    datalen = len(psdata)

    #calculate 2000 data offline and write to files
    dismatrix = getdisance(psdata,datalen)
    kdis = getk_distance(dismatrix,datalen,K)
    reach_mat = getreach_distance(dismatrix,datalen,kdis)
    lrd = getlrd(reach_mat,dismatrix,datalen,MinPts)

    #delete minflt
    sqlpsone = "select id,prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,stack_vm,totalfiles,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,totalsyscall,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from psinfo where state = 0"
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


def trainNormaiData(data,datalen,K,MinPts):
    dismatrix = getdisance(data,datalen)
    kdis = getk_distance(dismatrix,datalen,K)
    reach_mat = getreach_distance(dismatrix,datalen,kdis)
    lrd = getlrd(reach_mat,dismatrix,datalen,MinPts)
    print "train over!!"
    return kdis,lrd

def detectState(domname,sqltable):
    print 'enter detect state'
    K = 4
    MinPts = 4
    selectdb = DBHelper.DBHelper()
    sqlstate = "select 100-idl_cpu_in,usep_mem_in,usep_swap_in,pagein_in,pageout_in,interrupts1_in,interrupts2_in,interrupts3_in,loadavg1_in*100,loadavg5_in*100,loadavg15_in*100,int_sys_in*10,csw_sys_in*10,read_total_in/10,writ_total_in/10,ps_root_in,ps_other_in,use_cpu_out,recv_net_out,recv_netp_out,send_net_out,send_netp_out,lsmod_out,lsmod_out-lsmod0_in-lsmod1_in-lsmod2_in-lsmodother_in,ps_out-ps_root_in-ps_other_in from state where stat = 1"
    statedata = selectdb.oncesql(sqlstate)
    datalen = len(statedata)
    print "statedata len:",datalen
    kdis,lrd = trainNormaiData(statedata,datalen,K,MinPts)


    sqlstate = "select id,100-idl_cpu_in,usep_mem_in,usep_swap_in,pagein_in,pageout_in,interrupts1_in,interrupts2_in,interrupts3_in,loadavg1_in*100,loadavg5_in*100,loadavg15_in*100,int_sys_in*10,csw_sys_in*10,read_total_in/10,writ_total_in/10,ps_root_in,ps_other_in,use_cpu_out,recv_net_out,recv_netp_out,send_net_out,send_netp_out,lsmod_out,lsmod_out-lsmod0_in-lsmod1_in-lsmod2_in-lsmodother_in,ps_out-ps_root_in-ps_other_in from "+sqltable+" where stat = 0 and domname = '"+domname+"'"
    i = 0
    while True:
        i = i + 1       
        statedatanew = selectdb.oncesql(sqlstate)
        statedatanew = list(statedatanew)
        datanewlen = len(statedatanew)
        if datanewlen == 0:
            time.sleep(1)
            printlog("sleep 1!!"+str(i))
            continue
        else:
            printlog('start time:'+str(time.clock()))
            addnum = 0
            for a in range(0,datanewlen):
                alof = getoplof(statedata,K,MinPts,datalen,kdis,lrd,statedatanew[a][1:])
                changestate = ""
                if alof > 1.5 or alof < 0.7:
                    changestate = "update "+sqltable+" set stat = 1,lof="+str(alof)+" where id =" + str(statedatanew[a][0])
                    addnum = addnum + 1
                    selectdb.myupdate(changestate)

                    warningsql = "insert into warning(domname,class,sqlid,lof) values('%s','%s','%d','%f')"%(domname,"State Anomaly",statedatanew[a][0],alof)
                    selectdb.myupdate(warningsql)
                    warining_st(domname,statedatanew[a][0],alof)
                    #db.oncesql(changestate)
                    printlog('state add in '+str(statedatanew[a][0])+' '+str(alof)+' '+str(time.clock()))
                    #print 'state add in '+str(statedatanew[a][0])+' '+str(alof)+' '+str(time.clock())

                else:
                    changestate = "update "+sqltable+" set stat = 2,lof="+str(alof)+" where id =" + str(statedatanew[a][0])
                    selectdb.oncesql(changestate)
                    printlog('state move out '+str(statedatanew[a][0])+' '+str(alof)+' '+str(time.clock()))
                    #print 'state move out '+str(statedatanew[a][0])+' '+str(alof)+' '+str(time.clock())
            selectdb.allcommit()


def detectPsinfo(domname,sqltable):
    print 'enter detect psinfo'
    K = 4
    MinPts = 4
    db = DBHelper.DBHelper()
    print time.clock()
    #sqlps = "select layer,prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,stack_vm,totalfiles,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,totalsyscall,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from psinfo where state = 1 "
    sqlps = "select layer,prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,stack_vm,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from psinfo where state = 1 "
    psdata = db.oncesql(sqlps)
    print "psdata len:",len(psdata)
    datalen = len(psdata)
    kdis,lrd = trainNormaiData(psdata,datalen,K,MinPts)
    
    Globalvar.settrainover()
    print time.clock()
    #delete minflt
    #sqlpsone = "select id,psid,psname,layer,prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,stack_vm,totalfiles,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,totalsyscall,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from "+sqltable+" where state = 0 and domname = '"+domname+"'"
    sqlpsone = "select id,psid,psname,layer,prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,stack_vm,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from "+sqltable+" where state = 0 and domname = '"+domname+"'"
    i = 0
    while True:
        i = i+1
        psonedata = db.oncesql(sqlpsone)
        psonedata = list(psonedata)
        datanewlen = len(psonedata)
        #print "len:",datanewlen
        if datanewlen == 0:
            time.sleep(1)
            #print "sleep 1!!",i
            continue      
        #print time.clock()
        addnum = 0
        for a in range(0,datanewlen):
            alof = getoplof(psdata,K,MinPts,datalen,kdis,lrd,psonedata[a][3:])
            if alof > 1.5 or alof < 0.7:
                changestate = "update "+sqltable+" set state = 1,lof="+str(alof)+" where id =" + str(psonedata[a][0])
                addnum = addnum + 1
                db.myupdate(changestate)
                warningsql = "insert into warning(domname,class,sqlid,psid,pmname,lof) values('%s','%s','%d','%d','%s','%f')"%(domname,"Process Anomaly",psonedata[a][0],psonedata[a][1],psonedata[a][2],alof)
                db.myupdate(warningsql)
                warining_ps(domname,psonedata[a][1],psonedata[a][2],alof)
                #db.oncesql(changestate)
                printlog( str(psonedata[a][1])+" "+str(psonedata[a][2])+"(pssql:"+str(psonedata[a][0])+") add in "+str(alof)+" "+str(time.clock()))
                print str(psonedata[a][1])+" "+str(psonedata[a][2])+"(pssql:"+str(psonedata[a][0])+") add in "+str(alof)

            
            else:
                changestate = "update "+sqltable+" set state = 2,lof="+str(alof)+" where id =" + str(psonedata[a][0])
                db.oncesql(changestate)
                printlog( str(psonedata[a][1])+" "+str(psonedata[a][2])+"(pssql:"+str(psonedata[a][0])+") move out "+str(alof)+" "+str(time.clock()))
                print str(psonedata[a][1])+" "+str(psonedata[a][2])+"(pssql:"+str(psonedata[a][0])+") move out "+str(alof)

        db.allcommit()
        printlog('add: '+str(addnum))

def trainpsinfo(domname,sqltable,maxnum):
    print 'enter detect psinfo'
    K = 9
    MinPts = 9
    db = DBHelper.DBHelper()
    print time.clock()
    #sqlps = "select layer,prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,stack_vm,totalfiles,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,totalsyscall,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from psinfo where state = 1 "
    sqlps = "select layer,prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,stack_vm,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from npsinfo where state = 1 "
    psdata = db.oncesql(sqlps)
    print "psdata len:",len(psdata)
    datalen = len(psdata)
    kdis,lrd = trainNormaiData(psdata,datalen,K,MinPts)
    
    Globalvar.settrainover()
    print time.clock()
    #delete minflt
    #sqlpsone = "select id,psid,psname,layer,prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,stack_vm,totalfiles,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,totalsyscall,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from "+sqltable+" where state = 0 and domname = '"+domname+"'"
    sqlpsone = "select id,psid,psname,layer,prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,stack_vm,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from "+sqltable+" where id<"+str(maxnum)+" and state = 0 and domname = '"+domname+"'"
    i = 0
    while True:
        i = i+1
        psonedata = db.oncesql(sqlpsone)
        psonedata = list(psonedata)
        datanewlen = len(psonedata)
        #print "len:",datanewlen
        if datanewlen == 0:
            time.sleep(1)
            #print "sleep 1!!",i
            break      
        #print time.clock()
        addnum = 0
        for a in range(0,datanewlen):
            alof = getoplof(psdata,K,MinPts,datalen,kdis,lrd,psonedata[a][3:])
            if alof > 1.5 or alof < 0.7:
                changestate = "update "+sqltable+" set state = 1,lof="+str(alof)+" where id =" + str(psonedata[a][0])
                addnum = addnum + 1
                db.myupdate(changestate)           
                printlog( str(psonedata[a][1])+" "+str(psonedata[a][2])+"(pssql:"+str(psonedata[a][0])+") add in "+str(alof)+" "+str(time.clock()))
                print str(psonedata[a][1])+" "+str(psonedata[a][2])+"(pssql:"+str(psonedata[a][0])+") add in "+str(alof)
           
            else:
                changestate = "update "+sqltable+" set state = 2,lof="+str(alof)+" where id =" + str(psonedata[a][0])
                db.oncesql(changestate)
                printlog( str(psonedata[a][1])+" "+str(psonedata[a][2])+"(pssql:"+str(psonedata[a][0])+") move out "+str(alof)+" "+str(time.clock()))
                print str(psonedata[a][1])+" "+str(psonedata[a][2])+"(pssql:"+str(psonedata[a][0])+") move out "+str(alof)

        db.allcommit()
        printlog('add: '+str(addnum))

def trainmode(mode):
    
    db = DBHelper.DBHelper()
    sqlps = "select layer,prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,stack_vm,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation,state from trainset"
    psdata = db.oncesql(sqlps)
    psdata = list(psdata)
    X=[]
    Y=[]
    for l in psdata:
        X.append(l[:-1])
        Y.append(l[-1])
    print len(X),len(Y),len(X[0])
    #normalized_X = preprocessing.normalize(X)
    scaler = preprocessing.StandardScaler().fit(X)
    #scaled_X = preprocessing.scale(X)
    scaled_X = scaler.transform(X)
    score = cross_val_score(mode,scaled_X,Y)
    print score.mean()
    mode.fit(scaled_X,Y)
    #print mode.feature_importances_
    print "train over!"
    sqlpsone = "select id,psid,psname,layer,prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,stack_vm,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from testset where state = 0 and domname = '"+domname+"'"
    psonedata = db.oncesql(sqlpsone)
    psonedata = list(psonedata)
    datanewlen = len(psonedata)
    for a in psonedata:
        x_test = a[3:]
        x_id = a[0:2]
        x_stand = scaler.transform([x_test])
        y_test = mode.predict(x_stand)
        changestate = "update testset set state = "+str(y_test[0])+" where id =" + str(a[0])
        print a[0],a[2]," is ",y_test[0]
        db.oncesql(changestate)
    db.allcommit()

def selectmode(domname):
    from hpsklearn import HyperoptEstimator,random_forest,svc,knn,decision_tree,ada_boost,multinomial_nb,extra_trees
    db = DBHelper.DBHelper()
    sqlps = "select layer,prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,stack_vm,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation,state from trainset"
    psdata = db.oncesql(sqlps)
    psdata = list(psdata)
    X=[]
    Y=[]
    for l in psdata:
        X.append(l[:-1])
        Y.append(l[-1])
    print len(X),len(Y),len(X[0])
    scaler = preprocessing.StandardScaler().fit(X)
    scaled_X = scaler.transform(X)
    mode = HyperoptEstimator(classifier=random_forest('myrandom_forest'))
    #mode = ensemble.RandomForestClassifier()
    mode.fit(scaled_X,Y)
    print mode.best_model()
    #print mode()
    '''
    sqlps = "select layer,prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,stack_vm,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation,state from testset"
    psdata1 = db.oncesql(sqlps)
    psdata1 = list(psdata1)
    testX=[]
    testY=[]
    for l in psdata1:
        testX.append(l[:-1])
        testY.append(l[-1])
    testX_scale = scaler.transform(testX)
    print mode
    print(mode.score(testX_scale, testY))
    '''
    print "train over!"
    sqlpsone = "select id,psid,psname,layer,prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,stack_vm,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from abnpsinfo where id>3993 and state = 0 and domname = '"+domname+"'"
    psonedata = db.oncesql(sqlpsone)
    psonedata = list(psonedata)
    datanewlen = len(psonedata)
    for a in psonedata:
        x_test = a[3:]
        x_id = a[0:2]
        x_stand = scaler.transform([x_test])
        y_test = mode.predict(x_stand)
        changestate = "update abnpsinfo set state = "+str(y_test[0])+" where id =" + str(a[0])
        print a[0],a[2]," is ",y_test[0]
        db.oncesql(changestate)
    db.allcommit()

def detectpsmode(domname):
    from hpsklearn import HyperoptEstimator,random_forest,svc,knn,decision_tree,ada_boost,multinomial_nb,extra_trees
    db = DBHelper.DBHelper()
    sqlps = "select layer,prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,stack_vm,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation,state from trainset"
    psdata = db.oncesql(sqlps)
    psdata = list(psdata)
    X=[]
    Y=[]
    for l in psdata:
        X.append(l[:-1])
        Y.append(l[-1])
    print len(X),len(Y),len(X[0])
    scaler = preprocessing.StandardScaler().fit(X)
    scaled_X = scaler.transform(X)
    #mode = HyperoptEstimator(classifier=random_forest('myrandom_forest'))
    mode = ensemble.RandomForestClassifier()
    mode.fit(scaled_X,Y)
    #print mode.best_model()
    #print mode()
    Globalvar.settrainover()
    print "train over!"
    sqlpsone = "select id,psid,psname,layer,prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,stack_vm,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from testset where state = 0 and domname = '"+domname+"'"
    while True:
        psonedata = db.oncesql(sqlpsone)
        psonedata = list(psonedata)
        datanewlen = len(psonedata)
        if datanewlen==0:
            time.sleep(1)
            continue
        for a in psonedata:
            x_test = a[3:]
            x_id = a[0:2]
            x_stand = scaler.transform([x_test])
            y_test = mode.predict(x_stand)
            changestate = "update testset set state = "+str(y_test[0])+" where id =" + str(a[0])
            #print a[0],a[2]," is ",y_test[0]
            db.oncesql(changestate)
        db.allcommit()

def detectAllState1(domname):
    selectdb = DBHelper.DBHelper()
    sqlstate = "select id,100-idl_cpu_in,usep_mem_in,usep_swap_in,pagein_in,pageout_in,interrupts1_in,interrupts2_in,interrupts3_in,loadavg1_in*100,loadavg5_in*100,loadavg15_in*100,int_sys_in*10,csw_sys_in*10,read_total_in/10,writ_total_in/10,ps_root_in,ps_other_in,use_cpu_out,recv_net_out,recv_netp_out,send_net_out,send_netp_out,lsmod_out,lsmod_out-lsmod0_in-lsmod1_in-lsmod2_in-lsmodother_in,ps_out-ps_root_in-ps_other_in from nowstate where stat = 1 and domname = '"+domname+"'"
    statedata = selectdb.oncesql(sqlstate)
    statedata = list(statedata)
    print "len:",len(statedata),len(statedata[0])
    newdata = [x[1:] for x in statedata]
    newid = [x[0]for x in statedata]
    print len(newdata),len(newdata[0])
    statelof = getlof(newdata,4,4)
    print "lof len:",len(statelof)
    for  a in range(0,len(statedata)):
        print a,newid[a],statelof[a]

def detectAllPsinfo1(domname):
    selectdb = DBHelper.DBHelper()
    sqlps = "select id,layer,prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,stack_vm,totalfiles,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,totalsyscall,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from nowpsinfo where state = 1 and domname = '"+domname+"'"
    psdata = selectdb.oncesql(sqlps)
    psdata = list(psdata)
    print "len:",len(psdata),len(psdata[0])
    newdata = [x[1:] for x in psdata]
    newid = [x[0]for x in psdata]
    print len(newdata),len(newdata[0])
    pslof = getlof(newdata,4,4)
    print "lof len:",len(pslof)
    for  a in range(0,len(psdata)):
        print a,newid[a],pslof[a] 
def clearwarning():
    file_module = open('log/warning.log', 'r+')
    file_module.truncate()
    file_module.close()

def clearprintlog():
    file_module = open('log/print.log', 'r+')
    file_module.truncate()
    file_module.close()
def clearfilelog():
    file_module = open('log/file.log', 'r+')
    file_module.truncate()
    file_module.close()

def quit(signum,frame):
    Globalvar.setexit()
    exdata.stopServer(Globalvar.getser())
    print "you stop calcul_lof!!"
    sys.exit()

if __name__ =='__main__':
    try:
        signal.signal(signal.SIGINT, quit)
        signal.signal(signal.SIGTERM, quit)
        over = True
        ser=exdata.startServer()
        Globalvar.setser(ser)
        clearwarning()
        clearprintlog()
        clearfilelog()
        while over:
            print "enter your command:"
            command = raw_input()
            comands = command.split()
            if len(comands)==2:
                domname,operation = command.split()
            else:
                if command == 'q':
                    break
                else:
                    print "command error !"
                    continue
            operation = int(operation)
            print "(command:)",command

            if operation==1:
                print 'enter detect domU state'
                t0 = threading.Thread(target = detectState,args =[domname,'nowstate'],name='detectstate')
                t0.setDaemon(True)
                t0.start()
                t1 = threading.Thread(target = exdata.exdamain,args =[domname,'nowstate'],name='getstate')
                t1.setDaemon(True)
                t1.start()
            if operation==2:
                print 'enter detect domU psinfo'
                t0 = threading.Thread(target = detectPsinfo,args =[domname,'nowpsinfo'],name='detectPsinfo')
                t0.setDaemon(True)
                t0.start()
                while Globalvar.gettrainover():
                   time.sleep(1)
                t1 = threading.Thread(target = runpsinfo,args =[domname,str(operation)],name='getpsinfo')
                t1.setDaemon(True)
                t1.start()
            if operation==3:
                t0 = threading.Thread(target = detectAllState1,args =[domname],name='detectAllState1')
                t0.setDaemon(True)
                t0.start()
            if operation==4:
                t0 = threading.Thread(target = detectAllPsinfo1,args =[domname],name='detectAllPsinfo1')
                t0.setDaemon(True)
                t0.start()
            if operation==5:
                print 'enter collect domU state'
                t0 = threading.Thread(target = detectState,args =[domname,'state'],name='detectstate')
                t0.setDaemon(True)
                t0.start()
                t1 = threading.Thread(target = exdata.exdamain,args =[domname,'state'],name='getstate')
                t1.setDaemon(True)
                t1.start()
            if operation==6:
                print 'enter collect domU psinfo'
                t0 = threading.Thread(target = detectPsinfo,args =[domname,'psinfo'],name='detectPsinfo')
                t0.setDaemon(True)
                t0.start()
                while Globalvar.gettrainover():
                   time.sleep(1)
                t1 = threading.Thread(target = runpsinfo,args =[domname,str(operation)],name='getpsinfo')
                t1.setDaemon(True)
                t1.start()
            if operation==7:
                print 'enter train domU psinfo'
                a =  [1000,1500,2000,2500,3000,3500,4000,5000,6000,8000]
                for a1 in a:
                    trainpsinfo(domname,"npsinfo",a1)
                db = DBHelper.DBHelper()
                sqlps = "update npsinfo set state = 0 where id <501"
                psdata = db.oncesql(sqlps)
                trainpsinfo(domname,"npsinfo",1000)
                trainpsinfo(domname,"abnpsinfo",8000)
            if operation==8:
                print 'enter test domU psinfo'
                t0 = threading.Thread(target = trainpsinfo,args =[domname,'testset'],name='testpsinfo')
                t0.setDaemon(True)
                t0.start()
            if operation==9:
                mode = tree.DecisionTreeClassifier(criterion='entropy')
                print "tree.DecisionTreeClassifier(criterion=/'entropy/')"
                trainmode(mode)
            if operation==10:
                mode = linear_model.LogisticRegression()
                print "mode = linear_model.LogisticRegression"
                trainmode(mode)
            if operation==11:
                mode = naive_bayes.GaussianNB()
                print "mode = naive_bayes.GaussianNB"
                trainmode(mode)
            if operation==12:
                mode = neighbors.KNeighborsClassifier()
                print "mode = neighbors.KNeighborsClassifier"
                trainmode(mode)
            if operation==13:
                mode = svm.SVC()
                print "mode = svm.SVC"
                trainmode(mode)
            if operation==14:
                mode = ensemble.RandomForestClassifier()
                print "mode = ensemble.RandomForestClassifier"
                trainmode(mode)
            if operation==15:
                mode = ensemble.ExtraTreesClassifier()
                print "mode = ensemble.ExtraTreesClassifier"
                trainmode(mode)
            if operation==16:
                mode = ensemble.GradientBoostingClassifier()
                print "mode = ensemble.GradientBoostingClassifier"
                trainmode(mode)
            if operation==17:
                mode = ensemble.AdaBoostClassifier()
                print "mode = ensemble.AdaBoostClassifier"
                trainmode(mode)
            if operation==18:
                print "test select mode!"
                selectmode()
            if operation==19:
                print 'enter randome forest detect domU psinfo'
                t0 = threading.Thread(target = detectpsmode,args =[domname],name='detectPsmode')
                t0.setDaemon(True)
                t0.start()
                while Globalvar.gettrainover():
                   time.sleep(1)
                t1 = threading.Thread(target = runpsinfo,args =[domname,'2'],name='getpsinfo')
                t1.setDaemon(True)
                t1.start()
        Globalvar.setexit()
        exdata.stopServer(Globalvar.getser())
        
        print "exit calcul_lof normal!!"

    except Exception,exc:
        print "calcul_lof over",exc
    

    