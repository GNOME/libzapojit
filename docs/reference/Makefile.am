AUTOMAKE_OPTIONS = 1.6
DOC_MODULE = libzapojit-0.0
DOC_MAIN_SGML_FILE = $(DOC_MODULE)-docs.sgml
SCAN_OPTIONS = --rebuild-types
SCANGOBJ_OPTIONS =
DOC_SOURCE_DIR = $(top_srcdir)/src
MKDB_OPTIONS = --sgml-mode --output-format=xml --name-space=zpj

CFILE_GLOB = $(top_srcdir)/src/*.c
HFILE_GLOB = $(top_srcdir)/src/*.h

IGNORE_HFILES = \
	zpj.h \
	zpj-enums.h \
	$(NULL)

AM_CPPFLAGS = \
	$(SOUP_UNSTABLE_CPPFLAGS) \
	$(GLIB_CFLAGS) \
	$(GIO_CFLAGS) \
	$(JSON_CFLAGS) \
	$(REST_CFLAGS) \
	$(SOUP_CFLAGS) \
	$(NULL)

GTKDOC_LIBS = \
	$(top_builddir)/src/libzapojit-0.0.la \
	$(GLIB_LIBS) \
	$(GIO_LIBS) \
	$(JSON_LIBS) \
	$(REST_LIBS) \
	$(SOUP_LIBS) \
	$(NULL)

FIXREF_OPTIONS = \
	--extra-dir = $(GIO_PREFIX)/share/gtk-doc/html/gio \
	--extra-dir = $(GLIB_PREFIX)/share/gtk-doc/html/glib \
	--extra-dir = $(GLIB_PREFIX)/share/gtk-doc/html/gobject \
	--extra-dir = $(GOA_PREFIX)/share/gtk-doc/html/goa \
	--extra-dir = $(JSON_PREFIX)/share/gtk-doc/html/json-glib \
	--extra-dir = $(SOUP_PREFIX)/share/gtk-doc/html/libsoup-2.4 \
	--extra-dir = $(REST_PREFIX)/share/gtk-doc/html/rest-0.7 \
	$(NULL)

include $(top_srcdir)/gtk-doc.make
EXTRA_DIST += version.xml.in

gtk-docs-clean: clean
	cd $(srcdir) && rm -rf xml html
