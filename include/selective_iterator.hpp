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
	template <class Pred, class Iter, class MixerIn>
	struct selective_iterator_mixin
	: public std::iterator_traits<Iter> // borrow container's typedefs
	{
		bool have_begin;
		const Iter m_begin;
		const Iter m_end;
		const Pred& m_pred;
		typedef selective_iterator_mixin<Pred, Iter, MixerIn> self;

		// begin/end only -- COPIES BOTH
		selective_iterator_mixin(const Iter& begin, const Iter& end, const Pred& pred = Pred()) 
		: have_begin(true), m_begin(begin), m_end(end), m_pred(pred)
		{}
		// begin/end only -- MOVES BOTH
		selective_iterator_mixin(Iter&& begin, Iter&& end, const Pred& pred = Pred()) 
		: have_begin(true), m_begin(std::move(begin)), m_end(std::move(end)), m_pred(pred)
		{}
		
		Iter& base() { return static_cast<MixerIn *>(this)->base(); }
		const Iter& base() const { return static_cast<const MixerIn *>(this)->base(); }
		
		// assignment
		self& operator=(const self& arg)
		{
			assert(arg.m_begin == this->m_begin);
			assert(arg.m_end == this->m_end);
			assert(arg.m_pred == this->m_pred);
			return *this;
		}
		self& operator=(self&& arg) // move assignment
		{
			assert(arg.m_begin == this->m_begin);
			assert(arg.m_end == this->m_end);
			assert(arg.m_pred == this->m_pred);
			return *this;
		}
		// copy constructor
		selective_iterator_mixin(const self& arg) 
		: m_begin(arg.m_begin), m_end(arg.m_end), m_pred(arg.m_pred) {}
		// move constructor
		selective_iterator_mixin(self&& arg) 
		: m_begin(std::move(arg.m_begin)), m_end(std::move(arg.m_end)), 
		  m_pred(std::move(arg.m_pred))
		{}

		typename std::iterator_traits<Iter>::reference operator*() const { return *base(); }
		typename std::iterator_traits<Iter>::pointer operator->() const { return &*base(); }
		self& operator++() // prefix
		{ 	do { ++base(); } while (base() != m_end && !m_pred(base())); 
			return *this; }
		self  operator++(int) // postfix ++, so copying
		{ 	Iter tmp = base(); ++*this; // re-use prefix form 
			// return tmp
			return self(m_begin, m_end, tmp, m_pred); }
		self& operator--() // prefix --
		{	// our "--" is a bit broken if we didn't copy the BEGIN iterator
			do { --base(); } while ((!have_begin || (base() != m_begin)) && !m_pred(base()));
			// if we reached the beginning and pred is still false, 
			// this means undefined behaviour is okay (the user shouldn't have
			// called us) so stop at one memory location *beyond* begin
			if ((!have_begin || base() == m_begin) && !m_pred(base())) --base();
			return *this; }
		self operator--(int) // postfix, so copying
		{ 	Iter tmp = base(); --*this; 
			return self(m_begin, m_end, tmp, m_pred); }
		bool operator==(const self& arg) const { return base() == arg.base(); }
		bool operator!=(const self& arg) const { return base() != arg.base(); }
	};

	template <class Pred, class Iter>
	class selective_iterator 
		: public selective_iterator_mixin<Pred, Iter, selective_iterator<Pred, Iter> >
	{
		Iter m_iter;
		typedef selective_iterator<Pred, Iter> self;
		typedef selective_iterator_mixin<Pred, Iter, selective_iterator> super;
		void print_range()
		{
			std::cerr << "selective_iterator selecting over range: ";
			for (Iter i = this->m_begin; i != this->m_end; i++)
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
		: super(std::move(begin), std::move(end), pred), m_iter(std::move(val))
		{ 	assert(m_iter == this->m_end || this->m_pred(m_iter)); print_range(); }
		
		// begin/end only -- COPIES BOTH
		selective_iterator(const Iter& begin, const Iter& end, const Pred& pred = Pred()) 
		: super(begin, end, pred), m_iter(begin)
		{ 	while (m_iter != this->m_end && !this->m_pred(m_iter)) ++m_iter; print_range(); }

		// begin/end only -- MOVES BOTH
		selective_iterator(Iter&& begin, Iter&& end, const Pred& pred = Pred()) 
		: super(std::move(begin), std::move(end), pred), m_iter(this->m_begin)
		{ 	while (m_iter != this->m_end && !this->m_pred(m_iter)) ++m_iter; print_range(); }
		
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
			m_iter = std::move(m_iter);
			return *this;
		}	
		// copy constructor
		selective_iterator(const self& arg) 
		: super(arg) { m_iter = arg.m_iter; }
		// move constructor
		selective_iterator(self&& arg) 
		: super(std::move(arg)) { m_iter = std::move(arg.m_iter); }
	};

} // end namespace srk31

#endif
