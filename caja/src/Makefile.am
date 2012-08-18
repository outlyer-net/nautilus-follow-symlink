# $Id: Makefile.am 1307 2009-04-30 19:38:17Z toni $ #
#

# Required to correctly install the locale files (and to access them)
CPPFLAGS+=-DGNOMELOCALEDIR=\""$(datadir)/locale"\"

caja_extension_lib_LTLIBRARIES = libcaja-follow-symlink.la

# Must be installed in caja' extension dir
#caja_extension_libdir = $(libdir)/caja/extensions-1.0
# Cleaner way:
caja_extension_libdir = `pkg-config --variable=extensiondir libcaja-extension`

noinst_HEADERS=*.h caja-layer/libnautilus-extension/nautilus-menu-provider.h

libcaja_follow_symlink_la_SOURCES = follow-symlink.c nautilus-ext-follow-symlink.c
libcaja_follow_symlink_la_LIBADD = $(CAJA_EXTENSION_LIBS)
libcaja_follow_symlink_la_CFLAGS = $(CAJA_EXTENSION_CFLAGS)
# There's really no need to have versioned file names
libcaja_follow_symlink_la_LDFLAGS = -avoid-version
# See e.g. <http://www.seul.org/docs/autotut/#libtool> for version instructions
#libcaja_follow_symlink_la_LDFLAGS = -version-info 0:0:0

