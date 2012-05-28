lib_LTLIBRARIES = libzapojit-0.0.la

libzapojit_0_0_la_built_sources = \
	zpj-enums.c \
	zpj-enums.h \
	$(NULL)

# NB: We don't want to GISCAN zpj.h
libzapojit_0_0_la_headers = \
	zpj-authorization-domain.h \
	zpj-authorizer.h \
	zpj-goa-authorizer.h \
	zpj-skydrive.h \
	zpj-skydrive-entry.h \
	zpj-skydrive-file.h \
	zpj-skydrive-folder.h \
	zpj-skydrive-photo.h \
	$(NULL)

libzapojit_0_0_la_SOURCES = \
	$(libzapojit_0_0_la_built_sources) \
	zpj-authorization-domain.c \
	zpj-authorizer.c \
	zpj-goa-authorizer.c \
	zpj-skydrive.c \
	zpj-skydrive-entry.c \
	zpj-skydrive-file.c \
	zpj-skydrive-folder.c \
	zpj-skydrive-photo.c \
	$(NULL)

BUILT_SOURCES = \
	$(libzapojit_0_0_la_built_sources) \
	$(NULL)

libzapojitincludedir = $(includedir)/libzapojit-0.0

libzapojitsubincludedir = $(includedir)/libzapojit-0.0/zpj
libzapojitsubinclude_HEADERS = \
	zpj.h \
	$(libzapojit_0_0_la_headers) \
	$(NULL)

EXTRA_DIST = \
	zpj-enums.c.template \
	zpj-enums.h.template \
	$(NULL)

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""${datadir}/locale"\" \
	$(GOA_API_CHANGE_CPPFLAGS) \
	$(SOUP_UNSTABLE_CPPFLAGS) \
	$(GIO_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(GOA_CFLAGS) \
	$(JSON_CFLAGS) \
	$(REST_CFLAGS) \
	$(SOUP_CFLAGS) \
	$(NULL)

libzapojit_0_0_la_LDFLAGS =

libzapojit_0_0_la_LIBADD = \
	$(GIO_LIBS) \
	$(GLIB_LIBS) \
	$(GOA_LIBS) \
	$(JSON_LIBS) \
	$(REST_LIBS) \
	$(SOUP_LIBS) \
	$(NULL)

CLEANFILES =

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir)
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION

INTROSPECTION_GIRS += Zpj-0.0.gir

Zpj-0.0.gir: libzapojit-0.0.la Makefile
Zpj_0_0_gir_NAMESPACE = Zpj
Zpj_0_0_gir_VERSION = 0.0
Zpj_0_0_gir_CFLAGS = $(AM_CPPFLAGS)
Zpj_0_0_gir_LIBS = libzapojit-0.0.la
Zpj_0_0_gir_SCANNER_FLAGS = --warn-all --symbol-prefix=zpj --identifier-prefix=Zpj

Zpj_0_0_gir_INCLUDES = \
	Gio-2.0 \
	GLib-2.0 \
	Json-1.0 \
	Rest-0.7 \
	Soup-2.4 \
	$(NULL)

Zpj_0_0_gir_FILES = \
	$(addprefix $(srcdir)/,$(libzapojit_0_0_la_headers)) \
	$(addprefix $(srcdir)/,$(libzapojit_0_0_la_SOURCES)) \
	$(NULL)

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += \
	$(gir_DATA) \
	$(typelib_DATA) \
	$(NULL)

endif

if MAINTAINER_MODE

CLEANFILES += \
	xgen-etbc \
	xgen-etbh \
	xgen-tmc \
	xgen-tmh \
	$(NULL)

DISTCLEANFILES = \
	$(libzapojit_0_0_la_built_sources) \
	stamp-zpj-enums.h \
	stamp-zpj-marshalers.h \
	$(NULL)

zpj-enums.h: stamp-zpj-enums.h
	@true

stamp-zpj-enums.h: zpj-enums.h.template
	$(AM_V_GEN) ( \
		cd $(srcdir) \
		&& ( glib-mkenums \
			--template zpj-enums.h.template \
                        zpj-skydrive-entry.h \
                   ) >> xgen-etbh \
		&& ( cmp -s xgen-etbh zpj-enums.h || cp xgen-etbh zpj-enums.h ) \
		&& rm -f xgen-etbh \
		&& echo timestamp > $(@F) \
	)

zpj-enums.c: zpj-enums.c.template
	$(AM_V_GEN) ( \
		cd $(srcdir) \
		&& ( glib-mkenums \
			--template zpj-enums.c.template \
                        zpj-skydrive-entry.h \
                   ) >> xgen-etbc \
		&& ( cmp -s xgen-etbc zpj-enums.c || cp xgen-etbc zpj-enums.c ) \
		&& rm -f xgen-etbc \
	)

endif
