AC_CONFIG_MACRO_DIR([m4])
AC_PREREQ([2.69])
AC_INIT([libsrk31cxx], [0.1], [srk31@cl.cam.ac.uk])
AC_CONFIG_SRCDIR([include/srk31/array_slice_iterator.hpp])
CFLAGS="$CFLAGS -I$prefix/include $CFLAGS"
CXXFLAGS="$CXXFLAGS -I$prefix/include $CXXFLAGS"
LDFLAGS="$LDFLAGS -L$prefix/lib $LDFLAGS"
PKG_CONFIG_PATH="$prefix/lib/pkgconfig:$PKG_CONFIG_PATH"
export CFLAGS CXXFLAGS LDFLAGS PKG_CONFIG_PATH
LT_INIT
AC_LANG([C++])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_MAINTAINER_MODE
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])
AC_PROG_LN_S
AC_PROG_INSTALL
AC_CHECK_HEADERS([algorithm cassert cstdlib functional iostream iterator map memory stack string unistd.h vector], [], [AC_MSG_FAILURE([required standard headers: algorithm cassert cstdlib functional iostream iterator map memory stack string unistd.h vector])])

AX_BOOST_BASE

AC_CHECK_HEADER([boost/iostreams/filtering_stream.hpp], [], [AC_MSG_FAILURE([boost::iostreams is required])])
AC_HAVE_LIBRARY(boost_iostreams, [], [AC_MSG_FAILURE([boost::iostreams is required])])

# If the user (sanely) supplied _CXXFLAGS, and not _CFLAGS, 
# duplicate the latter to the former.  See rant about pkg-config in Makefile.am.
# We save the old _CFLAGS.
USER_LIBCXXFILENO_CFLAGS="$LIBCXXFILENO_CFLAGS"
LIBCXXFILENO_CFLAGS="$LIBCXXFILENO_CXXFLAGS"

# Now, all the information from pkg-config or from user _CXXFLAGS) is in _CFLAGS.
# We might also have USER_ _CFLAGS, if the user did what pkg.m4 stupidly told them.
# We use this information (i.e. propagate to _CXXFLAGS) only if the user *didn't* give us
# _CXXFLAGS.
AM_CONDITIONAL(SUPPLIED_LIBCXXFILENO_CFLAGS, [test x"$USER_LIBCXXFILENO_CFLAGS" != x && test x"$LIBCXXFILENO_CXXFLAGS" = x])

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([LIBCXXFILENO], [libcxxfileno])

AC_OUTPUT([Makefile include/Makefile src/Makefile libsrk31cxx.pc])
