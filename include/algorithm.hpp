#ifndef SRK31_ALGORITHM_HPP_
#define SRK31_ALGORITHM_HPP_

#include <algorithm>
#include <functional>
#include <iterator>

namespace srk31 {

// C++ predicate that is always true
template <typename T> struct True : std::unary_function<T, bool>
{
        bool operator()(const T& arg) const { return true; }
};

// function that counts the length of a sequence
template <class In> 
unsigned 
count(In first, In last)
{
	return std::count_if(first, last, True<typename std::iterator_traits<In>::value_type>());
}

// function that returns the length of an array
template<class T, size_t s> size_t array_len(T (&arg)[s]) { return s; }

// function that copies elements from a sequence if they match a predicate
/* copy_if should be in the standard algorithms, but it isn't. */
template<class In, class Out, class Pred>
Out copy_if(In first, In last, Out res, Pred p)
{
	while (first != last)
	{
		if (p(*first)) *res++ = *first;
		++first;
	}
	return res;
}	

} // end namespace srk31

#endif
