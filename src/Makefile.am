# $Id$ #
#

# Required to correctly install the locale files
CPPFLAGS+=-DGNOMELOCALEDIR=\""$(datadir)/locale"\"

nautilus_extension_lib_LTLIBRARIES = libnautilus-follow-symlink.la

# Must be installed in nautilus' extension dir
#nautilus_extension_libdir = $(libdir)/nautilus/extensions-1.0
# Cleaner way:
nautilus_extension_libdir = `pkg-config --variable=extensiondir libnautilus-extension`

libnautilus_follow_symlink_la_SOURCES = follow-symlink.c nautilus-ext-follow-symlink.c

# There's really no need to have versioned file names
libnautilus_follow_symlink_la_LDFLAGS = -avoid-version
# See e.g. <http://www.seul.org/docs/autotut/#libtool> for version instructions
#libnautilus_follow_symlink_la_LDFLAGS = -version-info 0:0:0

