AC_INIT(u2pnpd, 0.5)
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign no-dist-gzip dist-xz])
AM_SILENT_RULES([yes])
AC_PROG_CC
AC_GNU_SOURCE
AC_SYS_LARGEFILE

PKG_CHECK_MODULES(LIBUPNP, libupnp)

AC_OUTPUT([
    Makefile
])
