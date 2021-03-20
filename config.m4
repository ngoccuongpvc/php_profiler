dnl config.m4 for extension php_profiler

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

dnl If your extension references something external, use 'with':

dnl PHP_ARG_WITH([php_profiler],
dnl   [for php_profiler support],
dnl   [AS_HELP_STRING([--with-php_profiler],
dnl     [Include php_profiler support])])

dnl Otherwise use 'enable':

PHP_ARG_ENABLE([php_profiler],
  [whether to enable php_profiler support],
  [AS_HELP_STRING([--enable-php_profiler],
    [Enable php_profiler support])],
  [no])

if test "$PHP_PHP_PROFILER" != "no"; then
  dnl Write more examples of tests here...

  dnl Remove this code block if the library does not support pkg-config.
  dnl PKG_CHECK_MODULES([LIBFOO], [foo])
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBS, PHP_PROFILER_SHARED_LIBADD)

  dnl If you need to check for a particular library version using PKG_CHECK_MODULES,
  dnl you can use comparison operators. For example:
  dnl PKG_CHECK_MODULES([LIBFOO], [foo >= 1.2.3])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo < 3.4])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo = 1.2.3])

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-php_profiler -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/php_profiler.h"  # you most likely want to change this
  dnl if test -r $PHP_PHP_PROFILER/$SEARCH_FOR; then # path given as parameter
  dnl   PHP_PROFILER_DIR=$PHP_PHP_PROFILER
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for php_profiler files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       PHP_PROFILER_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$PHP_PROFILER_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the php_profiler distribution])
  dnl fi

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-php_profiler -> add include path
  dnl PHP_ADD_INCLUDE($PHP_PROFILER_DIR/include)

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-php_profiler -> check for lib and symbol presence
  dnl LIBNAME=PHP_PROFILER # you may want to change this
  dnl LIBSYMBOL=PHP_PROFILER # you most likely want to change this

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   AC_DEFINE(HAVE_PHP_PROFILER_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your php_profiler library.])
  dnl ], [
  dnl   $LIBFOO_LIBS
  dnl ])

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are not using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PHP_PROFILER_DIR/$PHP_LIBDIR, PHP_PROFILER_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_PHP_PROFILER_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your php_profiler library.])
  dnl ],[
  dnl   -L$PHP_PROFILER_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(PHP_PROFILER_SHARED_LIBADD)

  dnl In case of no dependencies
  AC_DEFINE(HAVE_PHP_PROFILER, 1, [ Have php_profiler support ])

  PHP_NEW_EXTENSION(php_profiler, php_profiler.c, $ext_shared)
fi
