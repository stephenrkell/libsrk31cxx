#ifndef SRK31_CONJOINING_ITERATOR_HPP_
#define SRK31_CONJOINING_ITERATOR_HPP_

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
struct conjoining_sequence;

template<typename Base>
struct conjoining_iterator
: public boost::iterator_adaptor<conjoining_iterator<Base>, // Derived
			Base, // Base
			typename std::iterator_traits<Base>::value_type,  // Value
			boost::use_default,
			typename std::iterator_traits<Base>::value_type  // Reference
		> 
{
private:
	typedef typename std::iterator_traits<Base>::value_type Value;
	typedef Base Iter;
	boost::shared_ptr<conjoining_sequence<Iter> > p_sequence;
	unsigned m_currently_in;
	bool alloc_sequence;
	
	typedef conjoining_iterator<Base> self;
public:
	// constructors
	// default constructor
	conjoining_iterator() : p_sequence() {}
	// one-sequence constructor
	conjoining_iterator(
		boost::shared_ptr<conjoining_sequence<Iter> > p_seq, 
		Iter val, unsigned val_in)
	 : conjoining_iterator::iterator_adaptor_(val), 
	   p_sequence(p_seq), 
	   m_currently_in(val_in) 
	{
		canonicalize_position();
	}
	// should be copy-constructible by compiler

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
		while (this->base() == p_sequence->m_ends.at(m_currently_in)
			&& this->base() != p_sequence->m_ultimate_end) 
		{
			this->base_reference() = p_sequence->m_begins.at(++m_currently_in);
		}
	}	

	void increment()
	{
		this->base_reference()++;
		while ( p_sequence->m_ends.at(m_currently_in) != 
			p_sequence->m_ultimate_end
			&& this->base() == p_sequence->m_ends.at(m_currently_in))
		{
			if (!(p_sequence->m_begins.size() > m_currently_in + 1))
			{
				/* Here we are suffering from:
				 * 
				 * this->base() equals our current end
				 * 
				 * AND our current end is the last in m_ends
				 *
				 * but is NOT equal to m_ultimate_end. It should be! */
				
				assert(false);
			}
			
			this->base_reference() = p_sequence->m_begins.at(++m_currently_in);
		}
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
		&& *(this->p_sequence) == *(arg.p_sequence); 
	}
	const typename std::iterator_traits<Base>::value_type dereference() const 
	{ return *this->base(); }
};

template <typename Iter>
struct conjoining_sequence : boost::enable_shared_from_this<conjoining_sequence<Iter> >
{
	std::vector<Iter> m_begins;
	std::vector<Iter> m_ends;
	Iter m_ultimate_end;
	bool m_initialized;

	typedef conjoining_sequence<Iter> self;
	
	// default constructor
	conjoining_sequence() : m_initialized(false) {}

	// one-sequence constructor
	conjoining_sequence(Iter begin1, Iter end1)
	 : m_ultimate_end(end1), m_initialized(true)
	{ m_begins.push_back(begin1); m_ends.push_back(end1); }
	// two-sequence constructor
	conjoining_sequence(Iter begin1, Iter end1, Iter begin2, Iter end2)
	 : m_ultimate_end(end2), m_initialized(true)
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

	// append utility, for building
	self& append(Iter begin, Iter end)
	{
		m_begins.push_back(begin);
		m_ends.push_back(end);
		m_ultimate_end = end;
		assert(end == m_ultimate_end);
		m_initialized = true;
		//std::cerr << "Appended " << (begin == end ? "empty" : "nonempty") << " sequence" << std::endl;
		return *this;
	}
	conjoining_iterator<Iter> begin() 
	{
		auto p_seq = this->shared_from_this();
		assert(&*p_seq == this);
		// FIXME: this doesn't work for empty m_begins
		assert(m_begins.size() > 0);
		return conjoining_iterator<Iter>(p_seq, m_begins.at(0), 0); 
	}
	conjoining_iterator<Iter> end() 
	{
		auto p_seq = this->shared_from_this();
		assert(&*p_seq == this);
		// FIXME: this doesn't work for empty m_ends
		assert(m_ends.size() > 0);
		return conjoining_iterator<Iter>(p_seq, m_ultimate_end, m_ends.size() - 1); 
	}
	bool operator==(const conjoining_sequence<Iter>& arg)
	{
		return this->m_initialized && arg.m_initialized && 
		this->m_begins == arg.m_begins &&
		this->m_ends == arg.m_ends &&
		this->m_ultimate_end == arg.m_ultimate_end; 
	}
	bool operator!=(const conjoining_sequence<Iter>& arg) { return !(*this == arg); }
	virtual ~conjoining_sequence() {
		// std::cerr << "Warning: destructed conjoining_sequence!" << std::endl; 
	}
};

} // end namespace srk31

#endif
