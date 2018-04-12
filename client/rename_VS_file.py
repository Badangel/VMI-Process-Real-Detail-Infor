import shutil 
import os,sys
import os.path
import subprocess,time
rootdir = "/home/ubuntu1604/Downloads/malware/sample/"

def rename(rootDir): 
    i = 0
    for lists in os.listdir(rootDir): 
        newname = lists.replace("VirusShare","VS")
        newname = newname[:15]
        os.rename(rootDir+lists,rootDir+newname)
        print lists,newname
    print 'over'

def find_copy(filelistDir,rootDir,copyDir): 
    file_list_addr = open(filelistDir, 'r+')
    file_lists = file_list_addr.readlines()
    file_list=[]
    for a in file_lists:
        a1 = a.replace("\r\n","")
        file_list.append(a1)
    file_list_addr.close()
    print file_list
    i = 0
    for lists in os.listdir(rootDir):
        #nowlist = lists[:15]
        if lists in file_list:
            i=i+1
            shutil.copyfile(rootDir+lists,copyDir+lists)
    print "add ",i," over!"

if __name__ =='__main__':
    rename(rootdir) 
    filelistdir="/home/ubuntu1604/Downloads/malware/vslist.txt"
    copydir="/home/ubuntu1604/Downloads/malware/vsup/"
    find_copy(filelistdir,rootdir,copydir)