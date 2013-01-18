#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([zen], [0.9.0], [darmacz@gmail.com], [zen])
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR([m4])

LT_PREREQ([2.4.2])
LT_INIT

AC_CONFIG_SRCDIR([zen/process/Pipeline.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL

# Checks for libraries.
AC_ARG_WITH([jsoncpp], AS_HELP_STRING([--without-jsoncpp], [Build without jsoncpp library]))

AS_IF([test "x$with_jsoncpp" != "xno"], [
AC_DEFINE(HAVE_JSONCPP, [1], [Define if jsoncpp library is to be used])
AC_ARG_WITH([jsoncpp-include-path],
  [AS_HELP_STRING([--with-jsoncpp-include-path],
    [location of the JSONcpp headers, defaults to /usr/include/jsoncpp])],
  [JSONCPP_CFLAGS="-I$withval"],
  [JSONCPP_CFLAGS='-I/usr/include/jsoncpp'])
AC_MSG_NOTICE([using jsoncpp library])
AC_SUBST([JSONCPP_CFLAGS])

AC_ARG_WITH([jsoncpp-lib-path],
  [AS_HELP_STRING([--with-jsoncpp-lib-path], [location of the JSONcpp libraries])],
  [JSONCPP_LIBS="-L$withval -ljsoncpp"],
  [JSONCPP_LIBS='-ljsoncpp'])
AC_SUBST([JSONCPP_LIBS])
])

# Get JSONcpp library and include locations

# Checks for header files.
AC_CHECK_HEADERS([unistd.h fcntl.h])
AC_CHECK_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([dup2 pipe2])
AC_FUNC_FORK

# global C++ flags, notably we want to use the c++11 standard
AC_LANG_PUSH(C++)
AX_CXX_COMPILE_STDCXX_11([noext])

# Generate output
AC_CONFIG_FILES([Makefile
		zen_process-0.9.pc
                m4/Makefile
                zen/Makefile
                zen/process/Makefile
                test/Makefile])

AC_OUTPUT
