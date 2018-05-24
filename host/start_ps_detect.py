#!/usr/bin/python
import threading,time,sys
from run_psinfo import runpsinfo
from state.DBHelper import DBHelper
from sklearn import linear_model
from sklearn import naive_bayes
from sklearn import neighbors
from sklearn import svm
from sklearn import ensemble
from sklearn import neural_network
from sklearn.model_selection import cross_val_score
from sklearn import preprocessing

from state.warning_log import *

def detect_ps(domname,sqltable):
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
    mode = ensemble.RandomForestClassifier()
    mode.fit(scaled_X,Y)

    getpsfactor_sql = "select ps_p,ps_m,ps_c,process_anomaly from response where domname = '"+domname+"'"
    psfactor = db.oncesql(getpsfactor_sql)
    
    psfactor1 = list(psfactor)
    pfactor = psfactor1[0]
    print pfactor
    ps_p = int(pfactor[0])
    ps_m = int(pfactor[1])
    ps_c = int(pfactor[2])
    ps_f = int(pfactor[3])
    factortotal = 0

    ps_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.psdetect', 'w')
    ps_detect.write("2")
    ps_detect.close()
    ps_Det = 2
    printlog("ps train over!")
    print "train over!"
    sqlpsone = "select id,psid,psname,layer,prio,inc_minflt,inc_majflt,inc_utime,inc_stime,mm_users,mm_count,stack_vm,unix, netlink,tcp,udp,tcpv6,eventfd,inotify, timerfd, signalfd, eventpoll, pipe, filenum,ps_control,file_rw,file_control,sys_control,mem_control,net_control,socket_control,user_control,ps_communcation from "+sqltable+" where state = 0 and domname = '"+domname+"'"
    while True:
        psonedata = db.oncesql(sqlpsone)
        psonedata = list(psonedata)
        datanewlen = len(psonedata)
        if datanewlen==0:
            ps_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.psdetect', 'r')
            ps_Det = int(ps_detect.readline())
            ps_detect.close()
            if ps_Det==0:
                break
            time.sleep(1)
            continue
        for a in psonedata:
            x_test = a[3:]
            x_id = a[0:2]
            x_stand = scaler.transform([x_test])
            y_test = mode.predict(x_stand)
            changestate = "update "+sqltable+" set state = "+str(y_test[0])+" where id =" + str(a[0])
            print a[0],a[2]," is ",y_test[0]
            db.myupdate(changestate)
            if y_test[0]==1:
                warningsql = "insert into warning(domname,class,sqlid,psid,pmname,lof) values('%s','%s','%d','%d','%s','%f')"%(domname,"Process Anomaly",a[0],a[1],a[2],1)
                factortotal = factortotal+ps_f
                db.myupdate(warningsql)
        setfactor_sql="update response set ps_factor = "+str(factortotal)+" where domname = '"+domname+"'"
        db.myupdate(setfactor_sql)
        db.allcommit()
        if factortotal>ps_p:
            responsevm(domname,1)
        else:
            if factortotal>ps_c:
                responsevm(domname,2)
            else:
                if factortotal>ps_m:
                    responsevm(domname,3)
        time.sleep(1)
        ps_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.psdetect', 'r')
        ps_Det = int(ps_detect.readline())
        ps_detect.close()
        if ps_Det==0:
            break

def start_ps(domname):
    t0 = threading.Thread(target = detect_ps,args =[domname,'nowpsinfo'],name='detect_ps')
    t0.setDaemon(False)
    t0.start()
    operation = 2
    
    printlog("ps collect will start!!")        
    t1 = threading.Thread(target = runpsinfo,args =[domname,str(operation)],name='getpsinfo')
    t1.setDaemon(False)
    t1.start()   
    
    printlog("ps start over!!")


if __name__=='__main__':
    domname = sys.argv[1]
    ps_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.psdetect', 'r')
    ps_Det = ps_detect.readline()
    ps_detect.close()
    print int(ps_Det)
    if int(ps_Det)==0:
        printlog("ps detect will start !! "+ps_Det)
        ps_detect = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/'+domname+'.psdetect', 'w')
        ps_detect.write("1")
        ps_detect.close()
        start_ps(domname)
