import pyvmi
import sys
import time

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


def get_linux_params(vmi):
    tasks_offset = vmi.get_offset("linux_tasks")
    name_offset = vmi.get_offset("linux_name") - tasks_offset
    pid_offset = vmi.get_offset("linux_pid") - tasks_offset

    list_head = vmi.translate_ksym2v("init_task")

    return (tasks_offset, name_offset, pid_offset, list_head)


def get_windows_params(vmi):
    tasks_offset = vmi.get_offset("win_tasks")
    name_offset = vmi.get_offset("win_pname") - tasks_offset
    pid_offset = vmi.get_offset("win_pid") - tasks_offset

    list_head = vmi.read_addr_ksym("PsInitialSystemProcess")

    return (tasks_offset, name_offset, pid_offset, list_head)


def processes(vmi):
    tasks_offset, name_offset, pid_offset, list_head = get_os_params(vmi)

    next_process = vmi.read_addr_va(list_head + tasks_offset, 0)
    list_head = next_process
    nump = 0
    #file_ps = open('ps', 'r')
   # ps_lines = file_ps.readlines()
    #if len(ps_lines)>0:
    #    print time.ctime(),ps_lines[len(ps_lines)-1]
    pslist = []

    while True:
        procname = vmi.read_str_va(next_process + name_offset, 0)
        pid = vmi.read_32_va(next_process + pid_offset, 0)
        next_process = vmi.read_addr_va(next_process, 0)
        
        nump += 1
        tmp = [nump,pid,procname]
        #print tmp
        pslist.append(tmp)
        #print "[%3d] %s"%(nump, procname),
        '''
        if nump<len(ps_lines)-1:
            psin = ps_lines[nump].split()
            if procname != psin[5]:
                print "[%3d] %5d %s"%(nump,pid, procname),psin[1],psin[5]
        else:
            print "[%3d] %5d %s"%(nump,pid, procname)
        if (list_head == next_process):
            tmp = nump
            tmp+=1
            while tmp<len(ps_lines):
                print "[%3d]"%(tmp),
                print "in vm ps: %s"%(ps_lines[tmp])
                tmp+=1
            print "inpsnum:%d outosnum:%d"%(tmp,nump)
            return nump
            '''
        if (list_head == next_process):
            return nump,pslist

def main(argv):
    vmi = pyvmi.init_complete("ubuntu1604")
    n = 1
    n,s = processes(vmi)
    print "len %d" % (n)
if __name__ == "__main__":
    main(sys.argv)
