#ifndef TASKINFO_H_INCLUDED
#define TASKINFO_H_INCLUDED


struct fdinfo{
    int pid;
    int fdnum;
    //socket in UNIX,NETLINK,TCP,UDP,TCPv6,other
    int socketinfo[6];
    //anon_inode in eventfd,inotify,timerfd,signalfd,other
    int anon_inodeinfo[5];
    int pipeinfo;
    int nullinfo;
    int numinfo;
};

typedef struct taskinfo{
    struct fdinfo *fsfdinfo;
    int tsparent;
    int tsgroupleader;
    int tsprio;
    unsigned int tsutime;
    unsigned int tsstime;
    uint64_t tsstart_time;
    uint64_t tsrealstart_time;
    unsigned long minflt;
    unsigned long majflt;

}TaskInfo;

#endif // TASKINFO_H_INCLUDED
