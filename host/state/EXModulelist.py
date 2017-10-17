
import pyvmi
import sys

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

   # list_head = vmi.translate_ksym2v("init_task")
    list_head = vmi.translate_ksym2v("modules")

    return (tasks_offset, name_offset, pid_offset, list_head)


def get_windows_params(vmi):
    tasks_offset = vmi.get_offset("win_tasks")
    name_offset = vmi.get_offset("win_pname") - tasks_offset
    pid_offset = vmi.get_offset("win_pid") - tasks_offset

    list_head = vmi.read_addr_ksym("PsInitialSystemProcess")

    return (tasks_offset, name_offset, pid_offset, list_head)


def modules(vmi):
    tasks_offset, name_offset, pid_offset, list_head = get_os_params(vmi)
    next_module = vmi.read_addr_va(list_head,0)
    list_head = next_module
  
    numm = 0

    while True:
        moduname = vmi.read_str_va(next_module + 16,0)
        #procname = vmi.read_str_va(next_process + name_offset, 0)
       # pid = vmi.read_32_va(next_process + pid_offset, 0)
        next_module = vmi.read_addr_va(next_module, 0)
        if (list_head == next_module):
            return numm
        #print "[%3d] %s"%(numm, moduname)
        numm += 1
        
'''
def main(argv):
    vmi = pyvmi.init_complete(argv[1])
    n = modules(vmi)
    print "len %d" % (n)
if __name__ == "__main__":
    main(sys.argv)
'''