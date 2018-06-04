#!/usr/bin/python
from state.DBHelper import DBHelper
import math
import Queue
import time,threading
import sys

import signal
from state.warning_log import *
from sklearn import tree

from sklearn import linear_model
from sklearn import naive_bayes
from sklearn import neighbors
from sklearn import svm
from sklearn import ensemble
from sklearn import neural_network
from sklearn.model_selection import cross_val_score
from sklearn import preprocessing,externals
from hpsklearn import HyperoptEstimator,random_forest,svc,knn,decision_tree,ada_boost,multinomial_nb,extra_trees,gradient_boosting,sgd

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
    #mode = externals.joblib.load("RF_train_model.m")

    mode = HyperoptEstimator(classifier=random_forest('myRF'))
    #mode = HyperoptEstimator(classifier=svc('mysvc'))
    #mode = HyperoptEstimator(classifier=extra_trees('myextra_trees'))
    #mode = svm.SVC()
    #mode = ensemble.ExtraTreesClassifier()
    #mode = neighbors.KNeighborsClassifier()
    #mode = ensemble.RandomForestClassifier()
    #mode = tree.DecisionTreeClassifier(criterion='gini')
    #mode = tree.DecisionTreeClassifier(criterion='entropy')
    #mode = ensemble.AdaBoostClassifier()
    #mode = neural_network.MLPClassifier()
    #mode = ensemble.GradientBoostingClassifier()
    #mode = naive_bayes.BernoulliNB()
    #mode = naive_bayes.GaussianNB()
    #mode = linear_model.LogisticRegression()
    mode.fit(scaled_X,Y)
    #print mode.best_model()
    #print mode()
    #with open("rf.dot",'w') as f:
       # f = tree.export_graphviz(mode,out_file=f)
    #externals.joblib.dump(mode,"RF_train_model.m")
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
    print "you stop calcul_lof!!"
    sys.exit()

if __name__ =='__main__':
    try:
        signal.signal(signal.SIGINT, quit)
        signal.signal(signal.SIGTERM, quit)
        over = True
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
                    over = False
                    break
                else:
                    print "command error !"
                    continue
            operation = int(operation)
            print "(command:)",command

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
                selectmode(domname)
            if operation==19:
                print 'enter randome forest detect domU psinfo'
                t0 = threading.Thread(target = detectpsmode,args =[domname],name='detectPsmode')
                t0.setDaemon(True)
                t0.start()
                
                t1 = threading.Thread(target = runpsinfo,args =[domname,'2'],name='getpsinfo')
                t1.setDaemon(True)
                t1.start()
        
        print "exit calcul_lof normal!!"

    except Exception,exc:
        print "calcul_lof over",exc
    

    