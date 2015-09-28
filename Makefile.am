AM_CFLAGS = -Wall -g

bin_PROGRAMS = u2pnpd

u2pnpd_SOURCES = \
    main.c \
    device_info.c \
    device_info.h \
    options.c \
    options.h

u2pnpd_CFLAGS = \
    $(LIBUPNP_CFLAGS)

u2pnpd_LDADD = \
    $(LIBUPNP_LIBS)

u2pnpddocdir = ${prefix}/doc/u2pnpd
u2pnpddoc_DATA = \
    README.md \
    AUTHORS \
    NEWS

EXTRA_DIST = $(u2pnpddoc_DATA)

# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(u2pnpddocdir)

CLEANFILES = *~
