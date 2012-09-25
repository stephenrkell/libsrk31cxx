#ifndef SRK31_SELECTIVE_ITERATOR_HPP_
#define SRK31_SELECTIVE_ITERATOR_HPP_

#include <iterator>
#include <iostream>
#include <cassert>

/* Most of the time, you should use boost::filter_iterator instead. 
 * But this class applies the predicate to the underlying iterator, 
 * not to the object yielded by dereferencing it. So this variant is
 * useful if there is some nontrivial cost to dereferencing the iterator,
 * and the predicate can be evaluated on the iterator without dereferencing. */

namespace srk31
{

	template <class Pred, class Iter>
	class selective_iterator 
    	: public std::iterator_traits<Iter> // borrow container's typedefs
	{
		bool have_begin;
		const Iter m_begin;
    	const Iter m_end;
    	const Pred& m_pred;
    	Iter m_iter;
    	typedef selective_iterator<Pred, Iter> self;
    	void print_range()
    	{
    		std::cerr << "selective_iterator selecting over range: ";
    		for (Iter i = m_begin; i != m_end; i++)
        	{
        		std::cerr << *i;
            	std::cerr << " ";
			}
        	//if (m_begin == m_end) assert(false);
        	std::cerr << std::endl;
    	}
	public:
    	// no default constructor
		
		// fully specifying constructor
    	selective_iterator(Iter&& begin, Iter&& end, Iter&& val, const Pred& pred = Pred()) 
    	: have_begin(true), m_begin(std::move(begin)), m_end(std::move(end)), m_pred(pred), m_iter(std::move(val))
    	{ 	assert(m_iter == m_end || m_pred(m_iter)); /*print_range();*/ }
    	
		// begin/end only -- COPIES begin
		selective_iterator(const Iter& begin, const Iter& end, const Pred& pred = Pred()) 
    	: have_begin(true), m_begin(begin), m_end(/*std::move(*/end/*)*/), m_pred(pred), m_iter(begin)
    	{ 	while (m_iter != m_end && !m_pred(m_iter)) ++m_iter; /*print_range();*/ }
// 		// begin/end only -- COPIES BOTH -- FIXME: understand why rvalue-ref overloads can't bind
// 		// to const&
// 		selective_iterator(const Iter& begin, const Iter& end, const Pred& pred = Pred()) 
//     	: have_begin(true), m_begin(begin), m_end(std::move(end)), m_pred(pred), m_iter(begin)
//     	{ 	while (m_iter != m_end && !m_pred(m_iter)) ++m_iter; /*print_range();*/ }

		// begin/end only -- avoids copying begin, by leaving it uninitialized
		selective_iterator(Iter&& begin, Iter&& end, const Pred& pred = Pred()) 
    	: have_begin(false), m_end(std::move(end)), m_pred(pred), m_iter(std::move(begin))
    	{ 	while (m_iter != m_end && !m_pred(m_iter)) ++m_iter; /*print_range();*/ }
		
		// how to support non-copying? get rid of "begin"?
		// Then we are "forward_selective_iterator".
		// We could just tolerate the copy of the first iterator of every subseq.
		// In that case I've already solved it above: just use the fully-specifying constructor.
		// BUT we want to support the forward-only case. 
		// No termination check.
		
		Iter& base() { return m_iter; }
		const Iter& base() const { return m_iter; }
		
    	// assignment
    	self& operator=(const self& arg)
    	{
    		assert(arg.m_begin == this->m_begin);
        	assert(arg.m_end == this->m_end);
        	assert(arg.m_pred == this->m_pred);
        	m_iter = arg.m_iter;
        	return *this;
    	}
		self& operator=(self&& arg) // move assignment
		{
    		assert(arg.m_begin == this->m_begin);
        	assert(arg.m_end == this->m_end);
        	assert(arg.m_pred == this->m_pred);
			m_iter = std::move(arg.m_iter);
			return &this;
		}
    	// copy constructor
    	selective_iterator(const self& arg) 
    	: m_begin(arg.m_begin), m_end(arg.m_end), m_pred(arg.m_pred), m_iter(arg.m_iter) {}
    	// move constructor
		selective_iterator(self&& arg) 
    	: m_begin(std::move(arg.m_begin)), m_end(std::move(arg.m_end)), 
		  m_pred(std::move(arg.m_pred)), m_iter(std::move(arg.m_iter)) {}

    	typename /*std::iterator_traits<Iter>::reference*/ Iter::value_type operator*() const { return *m_iter; }
    	typename std::iterator_traits<Iter>::pointer operator->() const { return &*m_iter; }
    	self& operator++() // prefix
    	{ 	do { ++m_iter; } while (m_iter != m_end && !m_pred(m_iter)); 
        	return *this; }
    	self  operator++(int) // postfix ++, so copying
    	{ 	Iter tmp = m_iter; ++*this; // re-use prefix form 
    		// return tmp
        	return self(m_begin, m_end, tmp, m_pred); }
    	self& operator--() // prefix --
    	{	// our "--" is a bit broken if we didn't copy the BEGIN iterator
			do { --m_iter; } while ((!have_begin || (m_iter != m_begin)) && !m_pred(m_iter));
        	// if we reached the beginning and pred is still false, 
        	// this means undefined behaviour is okay (the user shouldn't have
        	// called us) so stop at one memory location *beyond* begin
        	if ((!have_begin || m_iter == m_begin) && !m_pred(m_iter)) --m_iter;
        	return *this; }
    	self operator--(int) // postfix, so copying
    	{ 	Iter tmp = m_iter; --*this; 
    		//return tmp; 
        	return self(m_begin, m_end, tmp, m_pred); }
    	bool operator==(const self& arg) const { return this->m_iter == arg.m_iter; }
    	bool operator!=(const self& arg) const { return this->m_iter != arg.m_iter; }
	};

} // end namespace srk31

#endif
