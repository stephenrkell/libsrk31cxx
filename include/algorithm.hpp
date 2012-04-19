#ifndef SRK31_ALGORITHM_HPP_
#define SRK31_ALGORITHM_HPP_

#include <algorithm>
#include <functional>
#include <iterator>
#include <iostream> // for debugging

namespace srk31 {

// quick test whether an integer is a power of two
// inline bool is_power_of_two(int arg)
// {
// 	do
// 	{
// 		// 2^n looks like 1 when shifted left n times...
// 		if (arg == 1) return true;
// 		if (arg % 2 != 0) return false;
//		// now we know we have bottom bit zero.
// 		arg >>= 1;
// 	} while (arg != 0);
// 	return false;
// }

inline bool is_power_of_two(unsigned i)
{
        /* If we are a power of two, then one less than us                      
         * has a run of low-order bits set and no others set,                   
         * whereas we have a single (higher) bit set. So when                   
         * we AND, we get zero. In all other (non-power-of-two)                 
         * cases except zero, not all lower-order bits will                     
         * roll over between i-1 and i, so there will be a nonzero              
         * AND. */                                                              
        return (i != 0) && !(i & (i - 1));
}

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

/* This is modelled after std::upper_bound,
 * in Stroustrup C++PL 3e S18.7.2. */
/* This version avoids doing linear search to find the upper bound. */
template <typename For, typename T, typename Cmp >
For
greatest_le_from_upper_bound(For begin, For end, For upper_bound, const T& val, const Cmp& cmp)
{
	// we might have no elements
	if (begin == end) return end;

	// we might have no elements <= val, 
	// i.e. upper_bound == begin (but not end)
	// and not a direct hit (remember the upper bound is wrt "<" not "<=")
	if (upper_bound != end && upper_bound == begin
		&& cmp(val, *upper_bound)) return end;
	else
	{
		// we should never get an upper_bound <= val
		assert(upper_bound == end || cmp(val, *upper_bound)); 

		auto next_lower = upper_bound; --next_lower; 

		//std::cerr << "Next_lower has offset 0x" << std::hex << next_lower->first << std::dec << std::endl;
		//std::cerr << "Val has offset 0x" << std::hex << val.first << std::dec << std::endl;

		assert(cmp(*next_lower, val) || !cmp(val, *next_lower));
		return next_lower;
	}
}
/* This version finds the upper bound by itself, probably by linear search. */
template <typename For, typename T, typename Cmp >
For
greatest_le(For begin, For end, const T& val, const Cmp& cmp)
{
	auto upper_bound = std::upper_bound(begin, end, val, cmp);
	
	return greatest_le_from_upper_bound(begin, end, upper_bound, val, cmp);
}



// FIXME: add others (2-arg variant, least_ge x 2, greatest_lt, least_gt?)

} // end namespace srk31

#endif
