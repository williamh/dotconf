
dnl ## AM_PATH_DOTCONF([MINIMUM_VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT_FOUND]]])
dnl ##
dnl ##
AC_DEFUN(AM_PATH_DOTCONF,
[dnl
dnl Get the cflags and libs from the dotconf-config script
dnl

AC_ARG_WITH(dotconf-prefix,[  --with-dotconf-prefix=PFX   Prefix where dot.conf is installed (optional)], dotconf_config_prefix="$withval", dotconf_config_prefix="")

if test x$dotconf_config_prefix != x ; then
  dotconf_config_args="$dotconf_config_args --prefix=$dotconf_config_prefix"
  if test x${DOTCONF_CONFIG+set} != xset ; then
    DOTCONF_CONFIG=$dotconf_config_prefix/bin/dotconf-config
  fi
fi

AC_PATH_PROG(DOTCONF_CONFIG, dotconf-config, no)
min_dotconf_version=ifelse([$1], ,0.8.0,$1)
AC_MSG_CHECKING(for dot.conf - version >= $min_dotconf_version)
no_dotconf=""
if test "$DOTCONF_CONFIG" = "no" ; then
  no_dotconf=yes
else
  DOTCONF_CFLAGS=`$DOTCONF_CONFIG $dotconf_config_args --cflags`
  DOTCONF_LIBS=`$DOTCONF_CONFIG $dotconf_config_args --libs`

  dotconf_config_major_version=`$DOTCONF_CONFIG $dotconf_config_args --version | \
	sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
  dotconf_config_minor_version=`$DOTCONF_CONFIG $dotconf_config_args --version | \
	sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
  dotconf_config_micro_version=`$DOTCONF_CONFIG $dotconf_config_args --version | \
	sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

  dotconf_min_major_version=`echo $min_dotconf_version | \
	sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
  dotconf_min_minor_version=`echo $min_dotconf_version | \
	sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
  dotconf_min_micro_version=`echo $min_dotconf_version | \
	sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

  if test "$dotconf_config_major_version" -lt "$dotconf_min_major_version"; then
    no_dotconf=yes
  else
    if test "$dotconf_config_major_version" -eq "$dotconf_min_major_version" -a "$dotconf_config_minor_version" -lt "$dotconf_min_minor_version"; then
      no_dotconf=yes
    else
      if test "$dotconf_config_minor_version" -eq "$dotconf_min_minor_version" -a "$dotconf_config_micro_version" -lt "$dotconf_min_micro_version"; then
        no_dotconf=yes
      fi
    fi
  fi
fi

if test "x$no_dotconf" = x ; then
  AC_MSG_RESULT(yes)
  CFLAGS="$CFLAGS $DOTCONF_CFLAGS"
  LIBS="$DOTCONF_LIBS $LIBS"
  ifelse([$2], , :, [$2])
else
  AC_MSG_RESULT(no)
  if test "$DOTCONF_CONFIG" = "no" ; then
    echo "*** The dotconf-config script installed by dot.conf could not be found"
    echo "*** If dot.conf was installed in PREFIX, make sure PREFIX/bin is in"
    echo "*** your path, or set the DOTCONF_CONFIG environment variable to the"
    echo "*** full path to dotconf-config."
  fi
  GTK_CFLAGS=""
  GTK_LIBS=""
  ifelse([$3], , :, [$3])
fi

AC_SUBST(DOTCONF_CFLAGS)
AC_SUBST(DOTCONF_LIBS)

])

