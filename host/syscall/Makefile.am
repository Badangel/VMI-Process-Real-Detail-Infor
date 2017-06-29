## Source directory

SUBDIRS =

AM_CFLAGS = -I$(top_srcdir) $(GLIB_CFLAGS)
AM_CFLAGS += -Wall -Wextra

if DEBUG
AM_CFLAGS += -Werror
AM_CFLAGS += -Wno-missing-field-initializers -Wno-unused-parameter
endif

AM_LDFLAGS = -L$(top_builddir)/libvmi/.libs/
LDADD = -lvmi -lm $(LIBS) $(GLIB_LIBS)


bin_PROGRAMS = event-example
                 

event_example_SOURCES = event-example.c

