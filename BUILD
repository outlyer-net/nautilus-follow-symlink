
COMPILATION
===========

* If no ./configure exists (i.e. raw svn export), use the dist command:
	$ ./dist

* Configure
	$ ./configure --prefix=/usr

	Note that this being a nautilus extension, the --prefix is not really used
	at installation time as there's no real flexibility on where to install them
	(the appropiate place will be checked on install time). BUT it should match
	gnome's locale dir.

* Compile
	$ make

* Install
	$ make install


Of special interest:
	Pass -D_DEBUG to the precompiler to enable the debugging/verbose
	mode.
		CPPFLAGS="-D_DEBUG" ./configure
