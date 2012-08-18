#!/bin/sh -x

PATCH=kludges/libtool-honor-as-needed.patch
# Run this script to run the auto* tools in a correct order

echo "Trying to re-generate the build scripts..." >&2
test -f ltmain.sh && patch -R <"$PATCH"
autoreconf --install --symlink --verbose
patch < "$PATCH"
intltoolize --automake

