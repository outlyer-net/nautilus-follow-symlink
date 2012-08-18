
SUBDIRS = po src
ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
		po/ChangeLog \
		bootstrap.sh \
		@PACKAGE@.spec.in @PACKAGE@.spec \
		debian/changelog debian/control debian/compat debian/copyright \
		debian/dirs debian/docs debian/README.Debian debian/rules

# Extra files to get rid of when distcleaning
DISTCLEANFILES = \
	intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache

N_EXTENSIONDIR=`pkg-config --variable=extensiondir libnautilus-extension`

forceinstall: all
	pkg-config --atleast-version=2.21 libnautilus-extension
	sudo install -m644 src/.libs/libnautilus-follow-symlink.so $(N_EXTENSIONDIR)/
	-killall -9 nautilus

forceuninstall:
	sudo $(RM) $(N_EXTENSIONDIR)/libnautilus-follow-symlink.so
	-killall -9 nautilus

#ACLOCAL_AMFLAGS = -I m4

