dnl $Id$

AC_INIT(nautilus-follow-symlink, [1.1])
AC_CONFIG_SRCDIR([src/follow-symlink.c])

dnl Cross-compilation
AC_CANONICAL_SYSTEM
AC_CANONICAL_BUILD
AC_ARG_PROGRAM

AM_INIT_AUTOMAKE([1.7 foreign])
AC_REVISION(SVN $Rev$)

AC_CONFIG_HEADER(src/config.h)

dnl default FLAGS, if none provided (note to self: ':=' is not portable)
if test -z "$CPPFLAGS" ; then
	CPPFLAGS="-pedantic-errors"
fi
if test -z "$CFLAGS" ; then
	CFLAGS="-Werror -Winline"
fi

dnl Extra flags always set (might be overridden by user's)
dnl CPPFLAGS="$CPPFLAGS"
CFLAGS="-O2 -Wall -std=gnu99 $CFLAGS"
LDFLAGS="-Wl,--as-needed $LDFLAGS"

# libtool, DISABLE_STATIC should go before PROG_LIBTOOL
AM_DISABLE_STATIC
AM_PROG_LIBTOOL

AC_PROG_INSTALL
AC_LANG_C
AC_PROG_CC

# intltool
AC_PROG_INTLTOOL(0.18)
GETTEXT_PACKAGE=nautilus-follow-symlink
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [gettext domain])
ALL_LINGUAS="ca es" dnl Implied en
#AM_GNU_GETTEXT_VERSION dnl autoreconf only runs autopoint if this is present
AM_GLIB_GNU_GETTEXT

# string.h and stdlib.h are used
dnl defines STDC_HEADERS if ANSI-compliant headers are present
AC_HEADER_STDC
dnl Xref: http://www.seul.org/docs/autotut/#libtool
#AC_CHECK_HEADERS(sys/stat.h,,AC_MSG_ERROR([required header file missing]))

# glib-2.0: ensure it's present
dnl and set GLIB_CFLAGS and GLIB_LIBS
dnl The newer API in GNOME 2.22 requires 2.16 (maybe 2.14?),
dnl gio is the only module required (probably)
AM_PATH_GLIB_2_0(2.16.0, [], [], gio)

# pkg-config: ensure libnautilus-extension is found by pkg-config
PKG_CHECK_MODULES(NAUTILUS_EXTENSION, [libnautilus-extension])

#AC_DEFINE(_GNU_SOURCE)

dnl Incorporate the result of tests
CFLAGS="$CFLAGS $GLIB_CFLAGS $NAUTILUS_EXTENSION_CFLAGS"
LIBS="$LIBS $GLIB_LIBS $NAUTILUS_EXTENSION_LIBS"

cat <<EOF

##############################################################################
# Config:
#
# Flags:
#  CFLAGS   = $CFLAGS
#  CPPFLAGS = $CPPFLAGS
#  LDFLAGS  = $LDFLAGS
#  LIBS     = $LIBS
#  LDADD    = $LDADD
#  CC       = $CC
#  CPP      = $CPP
##############################################################################

EOF

AC_OUTPUT([Makefile
  src/Makefile
  po/Makefile.in
  nautilus-follow-symlink.spec
])
