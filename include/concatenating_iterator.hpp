#ifndef SRK31_CONCATENATING_ITERATOR_HPP_
#define SRK31_CONCATENATING_ITERATOR_HPP_

#include <iterator>
#include <vector>
#include <iostream>
#include <cassert>
#include <boost/iterator_adaptors.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace srk31
{

template<typename Iter>
struct concatenating_sequence;

template<typename Base>
struct concatenating_iterator
: public boost::iterator_adaptor<concatenating_iterator<Base>, // Derived
			Base, // Base
			typename std::iterator_traits<Base>::value_type,  // Value
			boost::use_default,
			typename std::iterator_traits<Base>::value_type  // Reference
		> 
{
private:
	typedef typename std::iterator_traits<Base>::value_type Value;
	typedef Base Iter;
	boost::shared_ptr<concatenating_sequence<Iter> > p_sequence;
	unsigned m_currently_in;
	bool alloc_sequence;
	
	typedef concatenating_iterator<Base> self;
public:
	// constructors
	// default constructor
	concatenating_iterator() : p_sequence() {}
	// one-sequence constructor
	concatenating_iterator(
		boost::shared_ptr<concatenating_sequence<Iter> > p_seq, 
		Iter val, unsigned val_in)
	 : concatenating_iterator::iterator_adaptor_(val), 
	   p_sequence(p_seq), 
	   m_currently_in(val_in) 
	{
		canonicalize_position();
	}
	// should be copy-constructible by compiler

	// get the underlying sequence
	boost::shared_ptr<concatenating_sequence<Iter> > 
	get_sequence() { return p_sequence; }
	// get the underlying sequence
	boost::shared_ptr<concatenating_sequence<Iter> > 
	get_sequence() const { return p_sequence; }

	unsigned get_currently_in() const 
	{ return m_currently_in; }

	// move-to-end utility
	void move_to_end()
	{
		this->base_reference() = p_sequence->m_ultimate_end;
	}
	
	void canonicalize_position()
	{
		// if we're currently at an end, but not the ultimate end,
		// advance to the beginning of the next non-empty sublist,
		// or the ultimate end
		
		// NOTE: this is tricky because sometimes our end() iterators
		// will not be distinct. So we have to look at m_currently_in
		// too.
		
		//while (this->base() == p_sequence->m_ends.at(m_currently_in)
		//	&& this->base() != p_sequence->m_ultimate_end) 
		//{
		//	this->base_reference() = p_sequence->m_begins.at(++m_currently_in);
		//}
		
		while (this->base() == p_sequence->m_ends.at(m_currently_in)
			&& m_currently_in != p_sequence->m_ends.size() - 1)
		{
			this->base_reference() = p_sequence->m_begins.at(++m_currently_in);
		}
	}	

	void increment()
	{
		this->base_reference()++;
		canonicalize_position();
	}

	void decrement()
	{	
		while (m_currently_in != 0 
			&& this->base() == p_sequence->m_begins.at(m_currently_in))
		{ this->base_reference() = p_sequence->m_ends.at(--m_currently_in); }

		this->base_reference()--;
		
		canonicalize_position();
	}
	bool equal(const self& arg) const { 
		return this->base() == arg.base()
		&& *(this->p_sequence) == *(arg.p_sequence)
		&& this->m_currently_in == arg.m_currently_in;
	}
	const typename std::iterator_traits<Base>::value_type dereference() const 
	{ auto& base = this->base();
	  return *base; }
};

/* Note Iter is the base iterator, not the concatenating_iterator. */
template <typename Iter>
struct concatenating_sequence : boost::enable_shared_from_this<concatenating_sequence<Iter> >
{
	std::vector<Iter> m_begins;
	std::vector<Iter> m_ends;
	bool m_initialized;

	typedef concatenating_sequence<Iter> self;
	typedef Iter iterator;
	
	// default constructor
	concatenating_sequence() : m_initialized(false) {}

	// one-sequence constructor
	concatenating_sequence(Iter begin1, Iter end1)
	 : m_initialized(true)
	{ m_begins.push_back(begin1); m_ends.push_back(end1); }
	// two-sequence constructor
	concatenating_sequence(Iter begin1, Iter end1, Iter begin2, Iter end2)
	 : m_initialized(true)
	{
		m_begins.push_back(begin1); m_ends.push_back(end1); 
		m_begins.push_back(begin2); m_ends.push_back(end2);
	}
	// should be copy-constructible by compiler

	/* We can efficiently compute is_empty (but not size()). */
	bool is_empty() const
	{
		if (m_begins.size() == 0) return true;
		else
		{
			for (unsigned i = 0; i < m_begins.size(); ++i)
			{
				assert(i < m_ends.size());
				if (m_begins.at(i) != m_ends.at(i)) return false;
			}
			return true;
		}
			
	}
	
	unsigned subsequences_count() const
	{
		assert(m_ends.size() == m_begins.size());
		return m_begins.size(); 
	}

	// append utility, for building
	self& append(Iter begin, Iter end)
	{
		m_begins.push_back(begin);
		m_ends.push_back(end);
		m_initialized = true;
		//std::cerr << "Appended " << (begin == end ? "empty" : "nonempty") << " sequence" << std::endl;
		return *this;
	}
	concatenating_iterator<Iter> begin() 
	{
		auto p_seq = this->shared_from_this();
		assert(&*p_seq == this);
		// FIXME: this doesn't work for empty m_begins
		assert(subsequences_count() > 0);
		return concatenating_iterator<Iter>(p_seq, m_begins.at(0), 0); 
	}
	concatenating_iterator<Iter> end() 
	{
		auto p_seq = this->shared_from_this();
		assert(&*p_seq == this);
		// FIXME: this doesn't work for empty m_ends
		assert(m_ends.size() > 0);
		return concatenating_iterator<Iter>(p_seq, m_ends.at(m_ends.size() - 1), m_ends.size() - 1); 
	}
	concatenating_iterator<Iter> at(const Iter& pos, unsigned currently_in)
	{
		return concatenating_iterator<Iter>(this->shared_from_this(), pos, currently_in);
	}
	
	bool operator==(const concatenating_sequence<Iter>& arg)
	{
		return this->m_initialized && arg.m_initialized && 
		this->m_begins == arg.m_begins &&
		this->m_ends == arg.m_ends; // &&
	}
	bool operator!=(const concatenating_sequence<Iter>& arg) { return !(*this == arg); }
	virtual ~concatenating_sequence() {
		// std::cerr << "Warning: destructed concatenating_sequence!" << std::endl; 
	}
};

} // end namespace srk31

#endif
