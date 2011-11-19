#ifndef LIBSRK31CXX_ORDINAL_HPP_
#define LIBSRK31CXX_ORDINAL_HPP_

#include <cassert>
#include <cmath>

namespace srk31 
{
	inline const char *ordinal_suffix(int num)
	{
		switch (abs(num % 10))
		{
			case 1: return "st";
			case 2: return "nd";
			case 3: return "rd";
			case 0: 
			case 4: 
			case 5: 
			case 6: 
			case 7: 
			case 8: 
			case 9: 
				return "th";
			default: assert(false);
		}
	}
}

#endif
