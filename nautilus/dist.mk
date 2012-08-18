#!/usr/bin/make -f

# $Id$

all:
	@echo "This file is used to aid in the setup of the build"
	@echo "environment, there are the following available targets"
	@echo " (use ./dist.mk <TARGET>):"
	@echo "    gen        Run the autotools"
	@echo "    clean      Remove the files created by gen and by the build process"
	@echo "    update-po  Update the language files with new translation or moved "
	@echo "               string locations (note it also updates the meta-timestamp)"
	@echo "               so it can get funny issuing this command when using CVS/SVN"
	@echo "    regen      'clean' then 'gen'"
	@echo "    force-gen  Like gen, but overwrite even up-to-date files"
	@echo
	@echo "The rest of important rules are provided by the generated Makefile"
	@echo " (i.e.: $$ make <TARGET>)"
	@echo "    harshtest (To be used during development) Copy the library to the system"
	@echo "              and kill nautilus to re-load it"
	@echo " and the standard GNU rules:"
	@echo "    dist      Create the distribution tarball"
	@echo "    distcheck Create tarball, check it builds and it installs. Add a DESTDIR"
	@echo "              or it will try to install in the system"

_gen_common:
	intltoolize --automake

gen:
	autoreconf --install --verbose --symlink
	$(MAKE) -f dist.mk _gen_common

force-gen:
	autoreconf --install --verbose --force --symlink
	$(MAKE) -f dist.mk _gen_common

update-po:
	cd po && intltool-update -p
	cd po && for file in *.po ; do \
		intltool-update --dist `basename $$file .po` ; \
	done

regen: clean gen

clean:
	-debclean
	-make distclean
	cd src && make clean || true
	#
	$(RM) -r autom4te.cache
	$(RM) config.* depcomp install-sh missing src/config.h src/config.h.in
	$(RM) aclocal.m4
	$(RM) configure Makefile Makefile.in src/Makefile src/Makefile.in
	$(RM) po/*.gmo po/Makefile po/Makefile.in po/POTFILES
	find . -name 'stamp-??' -exec rm {} \;
	# The following are moved thanks to Makefile.am (var DISTCLEANFILES)
	# (kept for situations in which make distclean fails)
	# Undo libtoolize
	$(RM) libtool.m4 lt*.m4 ltmain.sh libtool
	# Undo intltoolize
	$(RM) intltool*

