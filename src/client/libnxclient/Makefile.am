SOURCES = alarms.cpp dc.cpp events.cpp main.cpp objects.cpp session.cpp server.cpp

lib_LTLIBRARIES = libnxclient.la

libnxclient_la_SOURCES = $(SOURCES)
libnxclient_la_CPPFLAGS=-I@top_srcdir@/include -DLIBNXCLIENT_EXPORTS
libnxclient_la_LDFLAGS = -release $(NETXMS_LIBRARY_VERSION)
libnxclient_la_LIBADD = ../../libnetxms/libnetxms.la

EXTRA_DIST = \
	libnxclient.vcxproj libnxclient.vcxproj.filters \
	libnxclient.h \
	Makefile.w32
