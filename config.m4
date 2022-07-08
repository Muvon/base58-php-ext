dnl $Id$
dnl config.m4 for extension base91

sinclude(./autoconf/pecl.m4)
sinclude(./autoconf/php-executable.m4)

PECL_INIT([base91])

PHP_ARG_ENABLE(base91, whether to enable base91, [ --enable-base91   Enable base91])

if test "$PHP_BASE91" != "no"; then
  AC_DEFINE(HAVE_BASE91, 1, [whether base91 is enabled])
  PHP_NEW_EXTENSION(base91, base91.c lib/libbase91.c, $ext_shared)

  PHP_ADD_MAKEFILE_FRAGMENT
  PHP_INSTALL_HEADERS([ext/base91], [php_base91.h])
fi
