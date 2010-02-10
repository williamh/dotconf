dnl SQ_EXPAND_DIR(VARNAME, DIR)
dnl expands occurrences of ${prefix} and ${exec_prefix} in the given DIR,
dnl and assigns the resulting string to VARNAME
dnl example: SQ_EXPAND_DIR(LOCALEDIR, "$datadir/locale")
dnl eg, then: AC_DEFINE_UNQUOTED(LOCALEDIR, "$LOCALEDIR")
dnl by Alexandre Oliva
dnl from http://www.cygnus.com/ml/automake/1998-Aug/0040.html
AC_DEFUN(SQ_EXPAND_DIR, [
        $1=$2
        $1=`(
            test "x$prefix" = xNONE && prefix="$ac_default_prefix"
            test "x$exec_prefix" = xNONE && exec_prefix="${prefix}"
            eval echo \""[$]$1"\"
        )`
])


dnl ###############################################################
dnl   the following lines are taken from PHP (4.0b2)
dnl   for the mysql detection

dnl
dnl AC_PHP_ONCE(namespace, variable, code)
dnl
dnl execute code, if variable is not set in namespace
dnl
AC_DEFUN(AC_PHP_ONCE,[
  unique=`echo $ac_n "$2$ac_c" | tr -c -d a-zA-Z0-9`
  cmd="echo $ac_n \"\$$1$unique$ac_c\""
  if test -n "$unique" && test "`eval $cmd`" = "" ; then
    eval "$1$unique=set"
    $3
  fi
])

dnl
dnl AC_EXPAND_PATH(path, variable)
dnl
dnl expands path to an absolute path and assigns it to variable
dnl
AC_DEFUN(AC_EXPAND_PATH,[
  if test -z "$1" || echo "$1" | grep '^/' >/dev/null ; then
    $2="$1"
  else
    $2="`pwd`/$1"
  fi
])

dnl
dnl AC_ADD_LIBPATH(path)
dnl
dnl add a library to linkpath/runpath
dnl
AC_DEFUN(AC_ADD_LIBPATH,[
  AC_EXPAND_PATH($1, ai_p)
  AC_PHP_ONCE(LIBPATH, $ai_p, [
    EXTRA_LIBS="$EXTRA_LIBS -L$ai_p"
    RAW_RPATHS="$RAW_RPATHS ${raw_runpath_switch}$ai_p"
    if test -n "$APXS" ; then
      RPATHS="$RPATHS ${apxs_runpath_switch}$ai_p'"
    else
      RPATHS="$RPATHS ${ld_runpath_switch}$ai_p"
    fi
  ])
])



dnl
dnl AC_ADD_INCLUDE(path)
dnl
dnl add a include path
dnl
AC_DEFUN(AC_ADD_INCLUDE,[
  AC_EXPAND_PATH($1, ai_p)
  AC_PHP_ONCE(INCLUDEPATH, $ai_p, [
    INCLUDES="$INCLUDES -I$ai_p"
  ])
])

dnl
dnl AC_ADD_LIBRARY(library)
dnl
dnl add a library to the link line
dnl
AC_DEFUN(AC_ADD_LIBRARY,[
  AC_PHP_ONCE(LIBRARY, $1, [
    EXTRA_LIBS="$EXTRA_LIBS -l$1"
  ])
])

dnl
dnl AC_ADD_LIBRARY_WITH_PATH(library, path)
dnl
dnl add a library to the link line and path to linkpath/runpath
dnl
AC_DEFUN(AC_ADD_LIBRARY_WITH_PATH,[
  AC_ADD_LIBPATH($2)
  AC_ADD_LIBRARY($1)
])
dnl ###############################################################
dnl ###############################################################

