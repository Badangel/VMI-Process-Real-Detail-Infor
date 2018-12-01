#!/usr/bin/python
import pyvmi
import sys
import time
import socket
from state.warning_log import *

def get_os_params(vmi):
    ostype = vmi.get_ostype()

    if ostype.lower() == "windows":
        return get_windows_params(vmi)
    elif ostype.lower() == "linux":
        return get_linux_params(vmi)
    else:
        print("Sorry, {} ostype is not supported in this example yet.".format(
            ostype))
        exit(1)


def read_module_soure_list(vmi,offset,list_addr,source_or_target):
    list_head = vmi.read_addr_va(list_addr,0)
    #print "list_head:0x%x"%(list_head)
    #print "0x%x +8:0x%x +16:0x%x"%(list_head,vmi.read_addr_va(list_addr+8,0),vmi.read_addr_va(list_addr+16,0)),
    n = 0
    while True:
        tmp_module = 0
        if list_addr==list_head:
            break
        
        tmp_module = vmi.read_addr_va(list_head+16,0)-offset[source_or_target]-16
        #list_addr = tmp_next
        #print "%s:0x%x"%(source_or_target,tmp_module)
        modname = vmi.read_str_va(tmp_module + 24, 0)
        n= n + 1
        #print modname,
        list_head = vmi.read_addr_va(list_head,0)
    #print '!'
    return n

def read_module_target_list(vmi,offset,list_addr,source_or_target):
    list_head = vmi.read_addr_va(list_addr,0)
    #print "list_head:0x%x"%(list_head)
    #print "0x%x +8:0x%x +16:0x%x"%(list_head,vmi.read_addr_va(list_addr+8,0),vmi.read_addr_va(list_addr+16,0)),
    n = 0
    result=''
    while True:
        tmp_module = 0
        if list_addr==list_head:
            break        
        tmp_module = vmi.read_addr_va(list_head+24,0)#-offset[source_or_target]
        #list_addr = tmp_next
        #print "%s:0x%x"%(source_or_target,tmp_module)
        modname = vmi.read_str_va(tmp_module + 24, 0)
        n= n + 1
        #print modname,
        result=result+modname+','
        list_head = vmi.read_addr_va(list_head,0)
      
    return n,result

def modulelist(vmi,offset):
    next_module = vmi.translate_ksym2v("modules")
    list_head = next_module
    n = 0
    
    while True:
        tmp_next = 0
        tmp_next = vmi.read_addr_va(next_module,0)
        next_module = tmp_next-8
        if tmp_next==list_head:
            break
        modname = vmi.read_str_va(next_module + 24, 0)
        num_syms_mod = -1
        num_kp_mod = -1
        module_state = vmi.read_32_va(next_module,0)
        source_list_addr = vmi.read_addr_va(next_module + offset['source_list_offset'], 0)
        #print "source_adddr:0x%x"%(next_module + offset['source_list_offset'])
        source = read_module_soure_list(vmi,offset,next_module + offset['source_list_offset'],'source_list_offset')
        target_list_addr = vmi.read_addr_va(next_module + offset['target_list_offset'], 0)
        #print "target_adddr:0x%x"%(next_module + offset['source_list_offset'])
        target,target_module_name= read_module_target_list(vmi,offset,next_module + offset['target_list_offset'],'target_list_offset')
        n = n + 1
        print "%d&%s&%d&%d&%s"%(n,modname,source,target,target_module_name)
        #print "0x%x\n\n\n"%(next_module)
        next_module = tmp_next
        


def main(domname):
    printlog("module_list")
    vmi = pyvmi.init_complete(domname)
    config_file = open('/home/vmi/Downloads/code/VmiXen/host/config/'+'4.4.57'+'_offset.conf','r')
    lines = config_file.readlines()
    config_file.close()
    offset = {}
    for line in lines:
        line_list = line.split(' = ')
        if len(line_list)==2:
            offset[line_list[0]]=int(line_list[1][:-1])
   
    modulelist(vmi,offset)

if __name__ == "__main__":
    main(sys.argv[1])
