ACLOCAL_AMFLAGS = -I m4
SUBDIRS = include src
extra_DIST = libsrk31cxx.pc.in
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libsrk31cxx.pc

# pkg-config doesn't understand PKG_CXXFLAGS, but I'm buggered
# if I'm going to have my Makefiles use _CFLAGS to mean _CXXFLAGS.
# So, if we find we have _CFLAGS set for these, either from pkg.m4
# or because pkg.m4 told the user to set this var, and if we *don't*
# have _CXXFLAGS for these things, propagate. Only one of the USER_*_CFLAGS
# and *_CFLAGS should be non-empty. Note that if we got this far,
# we *must* have a _CFLAGS, even if the user (sanely) didn't call it that,
# because pkg-config will have complained if we didn't fake it up.
if SUPPLIED_LIBCXXFILENO_CFLAGS
LIBCXXFILENO_CXXFLAGS = $(USER_LIBCXXFILENO_CFLAGS)
else
LIBCXXFILENO_CXXFLAGS = $(LIBCXXFILENO_CFLAGS)
endif
