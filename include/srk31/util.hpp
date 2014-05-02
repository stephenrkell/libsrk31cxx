#ifndef LIBSRK31CXX_UTIL_HPP_
#define LIBSRK31CXX_UTIL_HPP_

/* This is a hack. "using" is a better C++11 feature to use, but g++ doesn't support it yet.  */
#define forward_constructors(base_typename, this_typename) \
template <typename... Args>  \
this_typename(Args&&... args) \
: base_typename(std::forward<Args>(args)...) {}


#endif
