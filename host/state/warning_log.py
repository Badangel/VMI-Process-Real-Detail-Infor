import time
import pyvmi
import DBHelper

def warining_w(vmi,domname, pid, pname):
    db = DBHelper.DBHelper()
    file_warning = open('log/warning.log', 'a')
    sql = "insert into warning(domname,class,psid,pmname) values('%s','%s','%d','%s')"%(domname,"Process Hide",pid,pname)
    data = db.oncesql(sql)
    file_warning.write(str(time.ctime())+' '+domname+' warning(ps): '+str(pid)+' '+str(pname)+' is hided!\n')
    file_warning.close()

def warining_ps(domname, pid, pname,lof):
    file_warning = open('log/warning.log', 'a')
    file_warning.write(str(time.ctime())+' '+domname+' warning(ps): '+str(pid)+' '+str(pname)+' lof = '+str(lof)+' is anomaly!\n')
    file_warning.close()


def warining_st(domname,sqlid,lof):
    file_warning = open('log/warning.log', 'a')
    file_warning.write(str(time.ctime())+' '+domname+' warning(state): state(sqlid='+str(sqlid)+') lof = '+str(lof)+' is anomaly!\n')
    file_warning.close()

def printlog(s):
    file_warning = open('log/print.log', 'a')
    file_warning.write('['+str(time.ctime())+']'+s+'\n')
    file_warning.close()

def cprintlog(s):
    file_warning = open('log/cprint.log', 'a')
    file_warning.write('['+str(time.ctime())+']'+s+'\n')
    file_warning.close()