import time,subprocess,sys
'''
file_ps = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/ubuntu1604add.ps', 'r+')
ps_lines = file_ps.readlines()
file_ps.close()
print ps_lines
print time.ctime(),time.time()
'''
import pexpect

if __name__ == '__main__':
    ip = '223.3.90.234'
    movehost = 'ubuntu1604test'
    mypassword = '309911'
    '''
    migrate_command = "xl migrate ubuntu1604test 223.3.90.234"
    #migrate_vm = subprocess.Popen(['xl','migrate','ubuntu1604test','223.3.90.234'], stdout = subprocess.PIPE)
    migrate_vm = subprocess.Popen(migrate_command,shell=True, stdout = subprocess.PIPE,stdin=subprocess.PIPE)
    time.sleep(2)
    migrate_vm.stdin.write("309911\n")
    print '309911\n'
    print 'iput over!!\n'
    migrate_vm.wait()
    print 'migrate over!'
    '''
    c = pexpect.spawn('xl migrate %s %s'%(movehost,ip))
    c.expect('root@%s\'s password:'%(ip))
    c.sendline (mypassword)
    c.interact()
    print 'migrate over!'