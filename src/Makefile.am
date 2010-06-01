lib_LTLIBRARIES = libdotconf.la
libdotconf_la_CFLAGS = $(WARNING_CFLAGS)
libdotconf_la_LDFLAGS = -version-info $(DC_CURRENT):$(DC_REVISION):$(DC_AGE)
libdotconf_la_SOURCES = dotconf.c \
	dotconf.h \
	dotconf_priv.h \
	readdir.c \
	readdir.h

include_HEADERS = dotconf.h
