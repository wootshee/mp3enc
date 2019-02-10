#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([mp3enc], [0.1], [denis.shtyrov@gmail.com])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_SRCDIR([src/mp3enc.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([memset])

AC_SUBST([AM_CXXFLAGS])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_CONFIG_SUBDIRS([extern/lame])
AC_OUTPUT
