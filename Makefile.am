
SUBDIRS = libpool src doc

bin_SCRIPTS = dotconf-config

m4datadir = $(datadir)/aclocal
m4data_DATA = dotconf.m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = dotconf.pc

EXTRA_DIST = bootstrap.sh $(m4data_DATA) $(pkgconfig_DATA).in dotconf_all.dsw \
	     dotconf_all.dsp dotconf.spec

dist-hook:
	cp -r $(top_srcdir)/examples $(top_distdir)/
	rm -irf `find $(top_distdir) -type d -name "CVS"`
