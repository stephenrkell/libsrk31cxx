#ifndef SRK31_CONJOINING_ITERATOR_HPP_
#define SRK31_CONJOINING_ITERATOR_HPP_

#include "concatenating_iterator.hpp"

/* Compatibility header. 
 * The new name for conjoining_sequence is concatenating_sequence. */

namespace srk31
{
	/* We can't typedef, so do the next best thing. */
	template<typename Iter>
	struct conjoining_sequence
		: public concatenating_sequence<Iter>
	{
		typedef concatenating_sequence<Iter> super;
		template <typename... Args>
		conjoining_sequence(Args&&... args)
		: super(std::forward<Args>(args)...) {}
	};

	template<typename Base>
	struct conjoining_iterator
		: public concatenating_iterator<Base>
	{
		typedef concatenating_iterator<Base> super;
		template <typename... Args>
		conjoining_iterator(Args&&... args)
		: super(std::forward<Args>(args)...) {}
	};
} // end namespace srk31

#endif
