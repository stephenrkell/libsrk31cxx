#ifndef LIBSRK31CXX_ARRAY_HPP_
#define LIBSRK31CXX_ARRAY_HPP_

#include <cstdlib>

namespace srk31 {

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
template <typename El>
class array<El, 0>
{
	// typedef El raw_array[];
	// typedef raw_array& raw_array_ref;
	// typedef raw_array const& raw_array_const_ref;
public:
	El& operator[](int ind) { if (ind != 0) abort(); return reinterpret_cast<El&>(this); }
	const El& operator[](int ind) const { if (ind != 0) abort(); return reinterpret_cast<El&>(this); }
	
	// constructors
	array(){}
	array(const array<El, 0>& o) 
	{
	}
	//array(const raw_array& o)
	//{
	//}
	
	//operator raw_array_ref() { return &(*this)[0]; }
	//operator raw_array_const_ref() const { return &(*this)[0]; }
};

template <typename El, int Dim>
class array_wrapper;

/* We will define a tail() template for array_wrappers. */
template <typename El, int Dim>
inline
array_wrapper<El, Dim - 1>
tail(array_wrapper<El, Dim>);

/* This is similar, but doesn't include any storage of its own. Rather,
 * it provides an interface around some preexisting array. It's necessary
 * when you want to assign to an array from an srk31::array or another array. */
template <typename El, int Dim>
class array_wrapper
{
	typedef El wrapped_type[Dim];
	
	wrapped_type& storage;
	
	typedef El raw_array[Dim];
	typedef raw_array& raw_array_ref;
	typedef raw_array const& raw_array_const_ref;
public:
	El& operator[](int ind) { return storage[ind]; }
	const El& operator[](int ind) const { return storage[ind]; }
	
	// constructors -- unlike array, these don't copy!
	
	// there is no default constructor, and no copy-from-object

	explicit array_wrapper(raw_array& o) : storage(o) {}
	explicit array_wrapper(srk31::array<El, Dim>& obj) : storage(obj) {}
	
	// there *is* an assignment operator
	array_wrapper& operator=(const wrapped_type& arg)
	{
		for (int i = 0; i < Dim; ++i) storage[i] = arg[i];
		return *this;
	}
	
	operator raw_array_ref() { return storage; }
	operator raw_array_const_ref() const { return storage; }
	
	template <typename El_, int Dim_>
	friend
	struct tail_t;
};
template <typename El>
class array_wrapper<El, 0>
{
	typedef void wrapped_type;
	
	wrapped_type *p_storage;
	
	// typedef El raw_array[];
	// typedef raw_array& raw_array_ref;
	// typedef raw_array const& raw_array_const_ref;
public:
	El& operator[](int ind) { if (ind != 0) abort(); return *reinterpret_cast<El *>(p_storage); }
	const El& operator[](int ind) const { if (ind != 0) abort(); return *reinterpret_cast<El *>(p_storage); }
	
	// constructors -- unlike array, these don't copy!
	
	// there is no default constructor, and no copy-from-object

	template <typename T>
	explicit array_wrapper(T& o) : p_storage(&o) {}
	
	explicit array_wrapper(srk31::array<El, 0>& obj) : p_storage(&obj) {}
	
	//operator raw_array_ref() { return storage; }
	// operator raw_array_const_ref() const { return storage; }
};

/* helper construct from array */
template <typename El, int Dim>
srk31::array_wrapper<El, Dim> make_array_wrapper(El (&from)[Dim])
{
	return array_wrapper<El, Dim>(from);
}

/* helper to make tail (suffix) array_wrapper */
template <typename El, int Dim>
struct tail_t
{
	static array_wrapper<El, Dim - 1>
	get(array_wrapper<El, Dim> arg) 
	{
		typedef El tail_wrapped_type[Dim - 1];
		return array_wrapper<El, Dim - 1>(*reinterpret_cast<tail_wrapped_type*>(&arg.storage[1]));
	}
};
template <typename El>
struct tail_t<El, 1>
{
	static array_wrapper<El, 0>
	get(array_wrapper<El, 1> arg) 
	{
		return array_wrapper<El, 0>(arg.storage[1]);
	}
};

template <typename El, int Dim>
inline
array_wrapper<El, Dim - 1>
tail(array_wrapper<El, Dim> arg)
{
	return tail_t<El, Dim>::get(arg);
}


} // end namespace srk31

/* To compile this test code into an executable, use 
 * $(CXX) -x c++ -DMAKE_TEST_PROGRAM ... */
#ifdef MAKE_TEST_PROGRAM
#include <iostream>
srk31::array<int, 3> function_returning_array()
{
	int blah[] = { 4, 5, 6 };
	return srk31::array<int, 3>(blah);
}

void function_taking_array(int args[3])
{
	std::cout << "Received a triple: " 
	<< args[0] << ", " << args[1] << ", " << args[2] << std::endl;
}

int main(void)
{
	srk31::array<int, 42> my_arr;
	
	// can we return an array?
	auto triple = function_returning_array();
	
	// can we pass it to a function taking a vanilla array?
	function_taking_array(triple);
	
	// can we assign to a local array? we need array_wrapper;
	int my_triple[3];
	srk31::make_array_wrapper(my_triple) = triple;

	std::cout << "My new triple has been assigned: " 
	<< my_triple[0] << ", " << my_triple[1] << ", " << my_triple[2] << std::endl;

	// can we get the tail of an existing array?
	srk31::array_wrapper<int, 3> triple_again(my_triple);
	std::cout << "Tail begins: " << srk31::tail(triple_again)[0] << std::endl;
	std::cout << "Tail continues: " << srk31::tail(srk31::tail(triple_again))[0] << std::endl;
	
	return 0;
}
#endif

#endif /* !defined LIBSRK31CXX_ARRAY_HPP_ */
