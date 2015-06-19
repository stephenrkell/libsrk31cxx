#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([include/srk31/array_slice_iterator.hpp])

# Checks for programs.
AC_PROG_CXX
AC_PROG_LN_S

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_LANG_CPLUSPLUS
AC_CHECK_LIB(c++fileno, [_Z6filenoIwSt11char_traitsIwEEiRKSt9basic_iosIT_T0_E], , AC_MSG_FAILURE([-lc++fileno is required]))

AC_CONFIG_FILES([config.mk])
AC_OUTPUT
