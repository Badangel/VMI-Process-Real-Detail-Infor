## Source directory

AUTOMAKE_OPTIONS=foreign
bin_PROGRAMS = vmiprocessinfo 

##Lack this 2 lines 
AM_LDFLAGS = -L$(top_builddir)/libvmi/.libs/
LDADD = -lvmi -lm $(LIBS) $(GLIB_LIBS)

vmiprocessinfo_SOURCES = myqueue.c myList.c acl.c vminit.c mysyscall.c mypsinfo.c mymodinfo.c myprint.c main.c
