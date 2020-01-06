#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([lxcpc], [0.0.1], [cesar.lombao@gmail.com])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_YACC
AC_PROG_LEX
AM_PROG_AR

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h string.h time.h])

# Checks for SDL2
SDL_VERSION=2.0
AM_PATH_SDL2($SDL_VERSION,:,AC_MSG_ERROR([*** No SDL $SDL_VERSION found please install it ***]))
AC_SUBST([SDL_CFLAGS])
CFLAGS="$CFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

# Check for lxz80
AC_ARG_WITH(lxz80-prefix,[  --with-lxz80-prefix=PFX   Prefix where LXZ80 is installed (optional],
            lxz80_prefix="$withval", lxz80_prefix="")
if test "x$lxz80_prefix" = x ; then
    PKG_CHECK_MODULES([LXZ80], [lxz80 >= 0.0.7])
else
    LXZ80_LIBS="-L${lxz80_prefix}/lib -llxz80"
    LXZ80_CFLAGS="-I${lxz80_include}/include"
fi



# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT8_T

# Checks for library functions.

AM_INIT_AUTOMAKE([1.11 -Wall -Werror foreign])
AC_CONFIG_FILES([src/Makefile Makefile])
LT_INIT([shared disable-static])
AC_OUTPUT

echo "
	LXCPC ${VERSION} has been configured for compilation
	Will be compiled into PREFIX: ${prefix}
	Using the following:
	LXZ80_LIBS: ${LXZ80_LIBS}
	LXZ80_CFLAGS: ${LXZ80_CFLAGS}
	SDL2_LIBS: ${SDL_LIBS}
	SDL2_CFLAGS: ${SDL_CFLAGS}
"
