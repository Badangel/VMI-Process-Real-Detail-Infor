#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(vmipsinfo, 2.0, dangyifei@seu.edu.cn)
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE(vmipsinfo,2.0)

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([m], [main])
# FIXME: Replace `main' with a function in `-lvmi':
AC_CHECK_LIB([vmi], [main])

AC_CHECK_LIB([mysqlclient], [main])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h inttypes.h stddef.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_PID_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC

#AC_CONFIG_FILES([Makefile])
AC_OUTPUT([Makefile])
