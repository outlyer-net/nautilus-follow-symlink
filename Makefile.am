
SUBDIRS = po src

EXTRA_DIST = \
		po/ChangeLog \
		intltool-extract.in \
		intltool-update.in \
		intltool-merge.in \
		@PACKAGE@.spec.in @PACKAGE@.spec \
		debian/changelog debian/control debian/compat debian/copyright \
		debian/dirs debian/docs debian/README.Debian debian/rules

# Extra files to get rid of when distcleaning
DISTCLEANFILES = \
	intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache

harshtest:
	make
	sudo cp src/.libs/libnautilus-follow-symlink.so /usr/lib/nautilus/extensions-3.0/
	-killall -9 nautilus

#ACLOCAL_AMFLAGS = -I m4

