#!/bin/bash
#
# This script shouldn't be executed unless new function calls
# or types are added

exec >layer.h
echo "Generating Nautilus to Caja preprocessor directives..." >&2

egrep --no-filename -o 'Nautilus[A-Z][^ ]*' *.c *.h |\
	sort | uniq |\
	sed -r -e 's/Nautilus([A-Z][^ ]*)/#define Nautilus\1    Caja\1/'

egrep --no-filename -o 'nautilus_[^(]*' *.c *.h | grep -v ' ' |\
	sort | uniq |\
	sed -r -e 's/nautilus_(.*)/#define nautilus_\1    caja_\1/'

egrep --no-filename -o 'NAUTILUS_TYPE_[A-Z_]*' *.c *.h |\
	sort | uniq |\
	sed -r -e 's/NAUTILUS_(.*)/#define NAUTILUS_\1    CAJA_\1/'

cat <<EOF
#undef NAUTILUS_COMMAND
#undef FSL_MSG_PREFIX
#define NAUTILUS_COMMAND "caja"
#define FSL_MSG_PREFIX "caja-follow-symlink"
EOF

