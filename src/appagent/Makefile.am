SOURCES = appagent.cpp client.cpp message.cpp tools.cpp

lib_LTLIBRARIES = libappagent.la

libappagent_la_SOURCES = $(SOURCES)
libappagent_la_CPPFLAGS=-I@top_srcdir@/include -DAPPAGENT_EXPORTS
libappagent_la_LDFLAGS = -release $(NETXMS_LIBRARY_VERSION)
libappagent_la_LIBADD = @top_srcdir@/src/libnetxms/libnetxms.la

EXTRA_DIST = Makefile.w32 appagent-internal.h appagent.vcxproj appagent.vcxproj.filters
