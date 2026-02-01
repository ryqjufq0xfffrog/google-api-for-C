# -*- Autoconf -*-
# generates ./configure

AC_PREREQ([2.71])
AC_INIT([google-api-for-C], [0.1], [ryqjufqvrnyhuxewucnpubvj@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux/m4])
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects foreign])
AM_PROG_AR([AC_MSG_ERROR([archiver not found])])
LT_PREREQ([2.4.0])
LT_INIT()
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([curl], [curl_version], [], [AC_MSG_ERROR([libcurl not found])])
AC_CHECK_LIB([json-c], [json_c_version], [], [AC_MSG_ERROR([json-c not found])])

# Checks for header files.
AC_CHECK_HEADERS([termios.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/samples/Makefile])
AC_OUTPUT
