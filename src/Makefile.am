
INCLUDES = -I. -I$(includedir)

include_HEADERS = dotconf.h
noinst_HEADERS = readdir.h
lib_LTLIBRARIES = libdotconf.la
libdotconf_la_SOURCES = dotconf.c readdir.c
libdotconf_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

