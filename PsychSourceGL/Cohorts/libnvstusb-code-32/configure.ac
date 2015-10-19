#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([libnvstub], [0.1])
AM_INIT_AUTOMAKE([-Wall -Wforeign])
AC_PROG_INSTALL
m4_ifdef([AM_PROG_AR], [AM_PROG_AR]) dnl Workaround for Automake 1.12

# Checks for programs.
AC_PROG_LIBTOOL

# Checks for libraries.
PKG_CHECK_MODULES([LIBUSB], [libusb-1.0 >= 1.0.0]) 
PKG_CHECK_MODULES([ILUT], [ILUT >= 1.7.0]) 
PKG_CHECK_MODULES([ILU], [ILU >= 1.7.0]) 
PKG_CHECK_MODULES([IL], [IL >= 1.7.0]) 
PKG_CHECK_MODULES([GL], [gl >= 7.7.0]) 
PKG_CHECK_MODULES([X11], [x11 >= 1.3.2, xxf86vm >= 1.1.0]) 
AC_CHECK_LIB(glut, glutMainLoop)

# Checks for header files.
AC_CHECK_HEADERS([stdint.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_UINT32_T

# Checks for library functions.
CFLAGS="-Werror -std=gnu99"

AC_MSG_CHECKING(whether to enable debugging)
debug_default="yes"

AC_ARG_ENABLE(debug, [  --enable-debug=[no/yes] turn on debugging
                       [default=$debug_default]],, enable_debug=$debug_default)
if test "x$enable_debug" = "xyes"; then
         CFLAGS="$CFLAGS -g -DNVSTUSB_DEBUG"
AC_MSG_RESULT(yes)
else
AC_MSG_RESULT(no)
fi


AC_CONFIG_FILES([Makefile src/Makefile tools/Makefile example/Makefile  pkgconfig/libnvstusb.pc])
AC_OUTPUT
