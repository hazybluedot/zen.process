#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([zen], [0.0.1], [darmacz@gmail.com], [zen])
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
AC_ARG_WITH([json], AS_HELP_STRING([--without-json], [Build without json library (default: test)]))

AS_IF([test "x$with_json" != "xno"], [
  PKG_CHECK_MODULES([JSON], [json >= 0.9])
])

# Need to push C++ for AX_BOOST macros to work properly
AC_LANG_PUSH(C++)

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
AX_CXX_COMPILE_STDCXX_11(noext)

# Generate output
AC_CONFIG_FILES([Makefile
                m4/Makefile
                zen/Makefile
                zen/process/Makefile
                test/Makefile])

AC_OUTPUT
