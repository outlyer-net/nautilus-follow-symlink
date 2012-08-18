
SUBDIRS = po src
ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
		po/ChangeLog README \
		po/POTFILES.in po/*.po \
		kludges/* \
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

C_EXTENSIONDIR=`pkg-config --variable=extensiondir libcaja-extension`

forceinstall: all
	pkg-config --atleast-version=1.2 libcaja-extension
	sudo install -m644 src/.libs/libcaja-follow-symlink.so $(C_EXTENSIONDIR)/
	-killall -9 caja

forceuninstall:
	sudo $(RM) $(C_EXTENSIONDIR)/libcaja-follow-symlink.so
	-killall -9 caja

#ACLOCAL_AMFLAGS = -I m4

