#ifndef LIBSRK31CXX_ARRAY_HPP_
#define LIBSRK31CXX_ARRAY_HPP_

/* This is (perhaps) the thinnest possible wrapper around C-style arrays in C++, 
 * designed with one goal only: to implement objects that may be returned by value
 * from a function, but are otherwise interchangeable with C-style arrays. */

template <typename El, int Dim>
class array 
{
	El a[Dim];
	typedef El raw_array[Dim];
	typedef raw_array& raw_array_ref;
	typedef raw_array const& raw_array_const_ref;
public:
	El& operator[](int ind) { return a[ind]; }
	const El& operator[](int ind) const { return a[ind]; }
	
	// constructors
	array(){}
	array(const array<El, Dim>& o) 
	{
		for (int i = 0; i < Dim; ++i) a[i] = o[i];
	}
	array(const raw_array& o)
	{
		for (int i = 0; i < Dim; ++i) a[i] = o[i];
	}
	
	operator raw_array_ref() { return a; }
	operator raw_array_const_ref() const { return a; }
	
	// I would like to have these, 
	// so we can just call f(my_array) for f(El *) { ... }, BUT
	// they make calls to function_taking_array (below) ambiguous
	// and the reference-based options (above) win, because
	// they are more type-safe (although more restricted)
	//operator El* () { return &a[0]; }
	//operator El const* () const { return &a[0]; }
};

/* To compile this test code into an executable, use 
 * $(CXX) -x c++ -DMAKE_TEST_PROGRAM ... */
#ifdef MAKE_TEST_PROGRAM
#include <iostream>
array<int, 3> function_returning_array()
{
	int blah[] = { 4, 5, 6 };
	return array<int, 3>(blah);
}

void function_taking_array(int args[3])
{
	std::cout << "Received a triple: " 
	<< args[0] << ", " << args[1] << ", " << args[2] << std::endl;
}

int main(void)
{
	array<int, 42> my_arr;
	
	// can we return an array?
	auto triple = function_returning_array();
	
	// can we pass it to a function taking a vanilla array?
	function_taking_array(triple);
	
	return 0;
}
#endif

#endif /* !defined LIBSRK31CXX_ARRAY_HPP_ */
