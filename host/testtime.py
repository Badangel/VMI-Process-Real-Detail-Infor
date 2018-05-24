import time

file_ps = open('/home/vmi/Downloads/code/VmiXen/host/tempfile/ubuntu1604add.ps', 'r+')
ps_lines = file_ps.readlines()
file_ps.close()
print ps_lines
print time.ctime(),time.time()