import time
import pyvmi
import DBHelper

def warining_w(vmi,domname, pid, pname):
    db = DBHelper.DBHelper()
    file_warning = open('warning.log', 'a')
    sql = "insert into warning(domname,class,psid,pmname) values('%s','%s','%d','%s')"%(domname,"Process Hide",pid,pname)
    data = db.oncesql(sql)
    file_warning.write(str(time.ctime())+' warning :'+str(pid)+' '+str(pname)+'is hided!\n')
    file_warning.close()