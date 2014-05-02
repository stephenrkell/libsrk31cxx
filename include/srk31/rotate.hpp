#ifndef SRK31CXX_ROTATE_HPP_
#define SRK31CXX_ROTATE_HPP_

template <typename Integral> 
inline Integral rotate_left(Integral val, unsigned n)
{
	return (val << n) | (val >> ((sizeof (Integral) * 8) - (signed) n));
} 

template <typename Integral> 
inline Integral rotate_right(Integral val, unsigned n)
{
	return (val >> n) | (val << ((sizeof (Integral) * 8) - (signed) n));
}

#endif
