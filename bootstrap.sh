#! /bin/bash

ACLOCAL=aclocal
test -n "$ACLOCAL_FLAGS" && ACLOCAL="$ACLOCAL $ACLOCAL_FLAGS"

$ACLOCAL
libtoolize --force --copy
autoheader
automake --add-missing
autoconf

