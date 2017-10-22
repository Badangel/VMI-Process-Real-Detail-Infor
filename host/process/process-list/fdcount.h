#ifndef FDCOUNT_H_INCLUDED
#define FDCOUNT_H_INCLUDED

struct fdinfo{
    int pid;
    int fdnum = 0;

    //socket in UNIX,NETLINK,TCP,UDP,TCPv6,other
    int socketinfo[6]={0,0,0,0,0,0};
    //anon_inode in eventfd,inotify,timerfd,signalfd,other
    int anon_inodeinfo[5]={0,0,0,0,0};
    int pipeinfo = 0;
    int nullinfo = 0;
    int numinfo = 0;

};


#endif // FDCOUNT_H_INCLUDED
