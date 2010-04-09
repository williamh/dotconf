SUBDIRS = libpool src doc

bin_SCRIPTS = dotconf-config

m4datadir = $(datadir)/aclocal
m4data_DATA = dotconf.m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = dotconf.pc

EXTRA_DIST = bootstrap.sh $(m4data_DATA) $(pkgconfig_DATA).in \
	     dotconf.spec examples
