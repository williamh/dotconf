#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([dotconf], [1.3], [w.d.hubbs@gmail.com])
AM_INIT_AUTOMAKE([foreign])
LT_PREREQ([2.2])
LT_INIT([dlopen])
AC_CONFIG_SRCDIR([src/dotconf.c])
AC_CONFIG_HEADERS([config.h])

# libtool version numbers for dotconf library:
#
# This is taken from the libtool manual.
# It is an algorithm, so follow all of the steps.
#
# 2. Only change this as the last step before a release.
# 3. If the library source has changed since the last release,
# increment revision.
# 4. If any interfaces have been added, removed or changed since the
# last release, increment current and set revision to 0.
# 5. If any interfaces have been added since the last release, increment age.
# 6. If any interfaces have been removed since the last release, set age to 0.
#
DC_CURRENT=0
DC_REVISION=1
DC_AGE=0
AC_SUBST([DC_CURRENT])
AC_SUBST([DC_REVISION])
AC_SUBST([DC_AGE])

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h malloc.h stdlib.h string.h strings.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_OFF_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([memset strcspn strdup strstr])

# Conditionally set extra compiler flags.
if test "$GCC" = yes; then
	WARNING_CFLAGS="-Wall"
else
	WARNING_CFLAGS="-errwarn=%all -errtags=yes -erroff=E_STATEMENT_NOT_REACHED"
fi
AC_SUBST(WARNING_CFLAGS)

AC_CONFIG_FILES([Makefile
                 dotconf.pc
                 doc/Makefile
                 examples/Makefile
                 src/Makefile])
AC_OUTPUT
