# $Id$ #
#

# Required to correctly install the locale files (and to access them)
CPPFLAGS+=-DGNOMELOCALEDIR=\""$(datadir)/locale"\"

nautilus_extension_lib_LTLIBRARIES = libnautilus-follow-symlink.la

# Must be installed in nautilus' extension dir
#nautilus_extension_libdir = $(libdir)/nautilus/extensions-1.0
# Cleaner way:
nautilus_extension_libdir = `pkg-config --variable=extensiondir libnautilus-extension`

noinst_HEADERS=*.h

libnautilus_follow_symlink_la_SOURCES = follow-symlink.c nautilus-ext-follow-symlink.c
libnautilus_follow_symlink_la_LIBADD = $(NAUTILUS_EXTENSION_LIBS)
libnautilus_follow_symlink_la_CFLAGS = $(NAUTILUS_EXTENSION_CFLAGS)
# There's really no need to have versioned file names
libnautilus_follow_symlink_la_LDFLAGS = -avoid-version
# See e.g. <http://www.seul.org/docs/autotut/#libtool> for version instructions
#libnautilus_follow_symlink_la_LDFLAGS = -version-info 0:0:0

