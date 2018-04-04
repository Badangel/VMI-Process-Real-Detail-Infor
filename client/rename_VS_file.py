import shutil 
import os,sys
import os.path
import subprocess,time
rootdir = "/home/ubuntu1604/Downloads/malware/sample/"

def rename(rootDir): 
    i = 0
    for lists in os.listdir(rootDir): 
        newname = lists.replace("VirusShare","VS")
        os.rename(rootDir+lists,rootDir+newname)
        print lists,newname
    print 'over'

if __name__ =='__main__':
    rename(rootdir) 