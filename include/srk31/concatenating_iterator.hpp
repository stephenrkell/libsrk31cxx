#ifndef SRK31_CONCATENATING_ITERATOR_HPP_
#define SRK31_CONCATENATING_ITERATOR_HPP_

#include <iterator>
#include <vector>
#include <iostream>
#include <cassert>
#include <boost/iterator_adaptors.hpp>
#include <memory>

namespace srk31
{

template<typename Iter, typename Value, typename Reference>
struct concatenating_sequence;

/* Tempted to try something a bit weird: two levels of CRTP. 
 * The first is with iterator_adaptor, to get our iterator-like
 * thing out of the simpler increment(), dereference() etc..
 * OH, WAIT. This is no good. If we use iterator_adaptor,
 * we contain an instance of the iterator. The point of the 
 * mixin is that we don't want to do that. Although possibly
 * the mixin is pointless -- what good does the separation
 * of the mixin from the iterator actually do? -- we do want
 * our iterator to be derived from Iter. */

template < typename Iter
         , typename MixerIn
         , typename Value = typename std::iterator_traits<Iter>::value_type
         , typename Reference = typename std::iterator_traits<Iter>::value_type&
         >
struct concatenating_iterator_mixin
{
private:
	typedef concatenating_iterator_mixin<Iter, MixerIn, Value, Reference> self;

	std::shared_ptr<concatenating_sequence<Iter, Value, Reference> > p_sequence;
	unsigned m_currently_in;
	bool initially_at_beginning;
	bool alloc_sequence;
	
public:
	// constructors
	concatenating_iterator_mixin() : p_sequence(), m_currently_in(0),
	  initially_at_beginning(false),
	  alloc_sequence(false)
	{}
	concatenating_iterator_mixin(self&& arg) 
	: p_sequence(arg.p_sequence), m_currently_in(std::move(arg.m_currently_in)),
	  initially_at_beginning(arg.initially_at_beginning),
	  alloc_sequence(arg.alloc_sequence)
	{}
	concatenating_iterator_mixin(const self& arg) 
	: p_sequence(arg.p_sequence), m_currently_in(arg.m_currently_in),
	  initially_at_beginning(arg.initially_at_beginning),
	  alloc_sequence(arg.alloc_sequence)
	{}	// one-sequence constructor
	concatenating_iterator_mixin(
		std::shared_ptr<
			concatenating_sequence<Iter, Value, Reference>
		> p_seq,
		Iter val, unsigned val_in)
	 : p_sequence(p_seq), 
	   m_currently_in(val_in),
	   initially_at_beginning(p_seq->m_begins.size() > 0 && val == p_seq->m_begins.at(0))
	{
		iter() = val;
		canonicalize_position();
	}
	// should be copy-constructible by compiler
	// assignment
	self& operator=(const self& arg)
	{
		this->p_sequence = arg.p_sequence;
		this->m_currently_in = arg.m_currently_in;
		this->initially_at_beginning = arg.initially_at_beginning;
		this->alloc_sequence = arg.alloc_sequence;
		iter() = arg.iter();
		return *this;
	}
	self& operator=(self&& arg) // move assignment
	{
		this->p_sequence = std::move(arg.p_sequence);
		this->m_currently_in = arg.m_currently_in;
		this->initially_at_beginning = arg.initially_at_beginning;
		this->alloc_sequence = arg.alloc_sequence;
		iter() = std::move(arg.iter());
		return *this;
	}
	
	const Iter& iter() const { return *static_cast<const MixerIn *>(this); }
		  Iter& iter()       { return *static_cast<      MixerIn *>(this); }

	// get the underlying sequence
	std::shared_ptr<concatenating_sequence<Iter, Value, Reference> > 
	get_sequence() { return p_sequence; }
	// get the underlying sequence
	std::shared_ptr<concatenating_sequence<Iter, Value, Reference> > 
	get_sequence() const { return p_sequence; }

	unsigned get_currently_in() const 
	{ return m_currently_in; }

	void canonicalize_position()
	{
		// if we're currently at an end, but not the ultimate end,
		// advance to the beginning of the next non-empty sublist,
		// or the ultimate end
		
		// NOTE: this is tricky because sometimes our end() iterators
		// will not be distinct. So we have to look at m_currently_in
		// too.
		
		while (this->iter() == p_sequence->m_ends.at(m_currently_in)
			&& m_currently_in != p_sequence->m_ends.size() - 1)
		{
			this->iter() = p_sequence->m_begins.at(++m_currently_in);
		}
	}	

	Reference operator*() const
	{ return *iter(); }
	Value *operator->() const 
	{ return &*iter(); }
	
	bool done_complete_pass() const
	{
		/* We're not a random-access iterator, so the only way to get to the end
		 * is to iterate along from the beginning or wherever we started. 
		 * To do a complete pass, simply start at the beginning.
		 * Intervening copy-assignments should copy the flag. */
		return initially_at_beginning && *this == p_sequence->end();
	}
	

private:
	void increment()
	{
		this->iter()++;
		canonicalize_position();
	}
	void decrement()
	{	
		while (m_currently_in != 0 
			&& this->iter() == p_sequence->m_begins.at(m_currently_in))
		{ this->iter() = p_sequence->m_ends.at(--m_currently_in); }
		this->iter()--;
		canonicalize_position();
	}
public:
	self& operator++() // prefix
	{
		increment();
		return *this;
	}
	self operator++(int) // postfix ++, so copying
	{
		self tmp = *this;
		tmp.increment();
		return std::move(tmp);
	}
	self& operator--() // prefix --
	{
		decrement();
		return *this;
	}
	self operator--(int) // postfix, so copying
	{
		self tmp = *this;
		tmp.decrement();
		return std::move(tmp);
	}
	
private:
	bool equal(const self& arg) const 
	{ 
		return this->iter() == arg.iter()
		&& *(this->p_sequence) == *(arg.p_sequence)
		&& this->m_currently_in == arg.m_currently_in;
	}
public:
	bool operator==(const self& arg) const { return this->equal(arg); }
	bool operator!=(const self& arg) const { return !this->equal(arg); }
};

template < class Iter
         , typename Value = typename std::iterator_traits<Iter>::value_type
         , typename Reference = typename std::iterator_traits<Iter>::value_type&
         >
class concatenating_iterator
	: public Iter
	, public concatenating_iterator_mixin< Iter
	                                     , concatenating_iterator<Iter, Value, Reference>
	                                     , Value
	                                     , Reference>
{
	typedef concatenating_iterator<Iter, Value, Reference> self;
	typedef concatenating_iterator_mixin<Iter, self, Value, Reference> super;
public:
	using super::operator++;
	using super::operator--;
	using super::operator!=;
	using super::operator==;
	using super::operator*;
	using super::operator->;
	// in  we borrow the copy constructor too
	using super::operator=;
	
	// which constructors does this import?
	using typename super::concatenating_iterator_mixin;
};


/* Note Iter is the base iterator, not the concatenating_iterator_mixin. */
template <typename Iter, 
	typename Value = typename std::iterator_traits<Iter>::value_type, 
	typename Reference = typename std::iterator_traits<Iter>::value_type& >
struct concatenating_sequence 
	: std::enable_shared_from_this<concatenating_sequence<Iter, Value, Reference> >
{
	std::vector<Iter> m_begins;
	std::vector<Iter> m_ends;
	bool m_initialized;

	typedef concatenating_sequence<Iter, Value, Reference> self;
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
	concatenating_iterator<Iter, Value, Reference> begin() 
	{
		auto p_seq = this->shared_from_this();
		assert(&*p_seq == this);
		// FIXME: this doesn't work for empty m_begins
		assert(subsequences_count() > 0);
		return concatenating_iterator<Iter, Value, Reference>(p_seq, m_begins.at(0), 0); 
	}
	concatenating_iterator<Iter, Value, Reference> end() 
	{
		auto p_seq = this->shared_from_this();
		assert(&*p_seq == this);
		// FIXME: this doesn't work for empty m_ends
		assert(m_ends.size() > 0);
		return concatenating_iterator<Iter, Value, Reference>(p_seq, m_ends.at(m_ends.size() - 1), m_ends.size() - 1); 
	}
	concatenating_iterator<Iter, Value, Reference> at(const Iter& pos, unsigned currently_in)
	{
		return concatenating_iterator<Iter, Value, Reference>(this->shared_from_this(), pos, currently_in);
	}
	
	bool operator==(const concatenating_sequence<Iter, Value, Reference>& arg)
	{
		return this->m_initialized && arg.m_initialized && 
		this->m_begins == arg.m_begins &&
		this->m_ends == arg.m_ends; // &&
	}
	bool operator!=(const concatenating_sequence<Iter, Value, Reference>& arg) { return !(*this == arg); }
	virtual ~concatenating_sequence() {
		// std::cerr << "Warning: destructed concatenating_sequence!" << std::endl; 
	}
};

} // end namespace srk31

#endif
