
COMPILER NOTES
==============

* This program is meant to be built whith gcc.
While where possible care has been taken to not rely on GCC, the code is (and
will remain) only tested to build with it. Additionally, future versions
might use GCC's extension to the C syntax.

* Error-on-warning is used by default, keep in mind in case some future
gcc version gets more picky.

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
