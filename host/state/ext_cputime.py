import libvirt

def getcputime(dom):
    dominfo = dom.info()
    return dominfo[4]