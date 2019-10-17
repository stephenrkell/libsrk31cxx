#ifndef SRK31_SELECTIVE_ITERATOR_HPP_
#define SRK31_SELECTIVE_ITERATOR_HPP_

#include <iterator>
#include <iostream>
#include <cassert>

/* Most of the time, you should use boost::filter_iterator instead. 
 * But this class applies the predicate to the underlying iterator, 
 * not to the object yielded by dereferencing it. So this variant is
 * useful if there is some nontrivial cost to dereferencing the iterator,
 * and the predicate can be evaluated on the iterator without dereferencing.
 * 
 * Another benefit we have is that we provide the core of the iterator as
 * a CRTP mixin. The idea here is to avoid boost:: iterators' need for base()
 * calls if you want to access methods on the underlying iterator.
 *
 * To get iterator class J backed by iterators of class I, 
 * rather than creating a completely new iterator class with an encapsulated
 * J, like this:
 *
 *      typedef selective_iterator<Pred, I> J
 * 
 * ... we derive a new class
 * 
 *      struct J : I, selective_iterator_mixin<Pred, I, J> {};
 * 
 * such that J is both an I and a selective I.
 *
 * Then there's no need to call base(). The utility methods on I are available
 * directly, but the relevant behaviour is overloaded in the mixin. 
 * 
 * PROBLEM: do we want to leave room for impls where the base iterator
 * instance is actually not "this" but a contained field? Hmm. NO!
 * 
 * PROBLEM: operator overload ambiguity: how to tell the compiler
 * that the selective_iterator_mixin's definitions take precedence
 * over I's? */

namespace srk31
{
	template <class Pred, class Iter, class MixerIn>
	struct selective_iterator_mixin
	 : public std::iterator_traits<Iter> // borrow container's typedefs
	{
		bool have_begin;
		/* const */ Iter m_begin;
		/* const */ Iter m_end;
		Pred m_pred;
		typedef selective_iterator_mixin<Pred, Iter, MixerIn> self;
		typedef MixerIn super;
		/* We don't have m_iter. The idea is that "*this", 
		 * appropriately static_cast'd, is m_iter.
		 * 
		 * This has some surprising consequences. Although we
		 * want to define various operators and constructors
		 * here, so that the mixer-in doesn't have to (it can just
		 * do "using" them), it is confusing in some cases. 
		 * Constructors, including copy and move constructors,
		 * become responsible for initialising/copying/moving the
		 * *superobject*'s fields. I *think* this works.... */
		const Iter& iter() const { return *static_cast<const MixerIn *>(this); }
		      Iter& iter()       { return *static_cast<      MixerIn *>(this); }

		void print_range()
		{
// 			std::cerr << "selective_iterator_mixin selecting over range: ";
// 			unsigned count = 0;
// 			for (Iter i = this->m_begin; i != this->m_end; i++)
// 			{
// 				//std::cerr << *i;
// 				//std::cerr << " ";
// 				++count;
// 			}
// 			std::cerr << "of " << count << " elements" << std::endl;
		}

		// how to support non-copying? get rid of "begin"?
		// Then we are "forward_selective_iterator".
		// We could just tolerate the copy of the first iterator of every subseq.
		// In that case I've already solved it above: just use the fully-specifying constructor.
		// BUT we want to support the forward-only case. 
		// No termination check.

		/* These constructors do not match copy/move signatures,
		 * so we are okay to initialise iter() here (cf. see below for copy/move).
		 * We are effectively writing over the top of a default-constructed 
		 * subobject. Be careful with the rvalue refs, though. */

		// fully specifying constructor
		selective_iterator_mixin(Iter&& begin, Iter&& end, Iter&& val, const Pred& pred = Pred()) 
		: m_begin(std::move(begin)), m_end(std::move(end)), m_pred(pred)
		{
			//std::cerr << "Three-arg moving constructor called for " << this << std::endl;
			iter() = std::move(val);
			assert(static_cast<Iter&>(*this) == this->m_end
			|| this->m_pred(static_cast<Iter&>(*this)));
			print_range(); 
		}
		
		// fully specifying constructor
		selective_iterator_mixin(const Iter& begin, const Iter& end, const Iter& val, const Pred& pred = Pred()) 
		: m_begin(begin), m_end(end), m_pred(pred)
		{
			//std::cerr << "Three-arg non-moving constructor called for " << this << std::endl;
			iter() = val;
			assert(static_cast<Iter&>(*this) == this->m_end
			|| this->m_pred(static_cast<Iter&>(*this)));
			print_range(); 
		}

		// begin/end only -- COPIES BOTH
		selective_iterator_mixin(const Iter& begin, const Iter& end, const Pred& pred = Pred()) 
		: have_begin(true), m_begin(begin), m_end(end), m_pred(pred)
		{
			//std::cerr << "Two-arg non-moving constructor called for " << this << std::endl;
			iter() = begin;
			while (iter() != this->m_end
				&& !this->m_pred(iter()))
			{
				//std::cerr << "Advancing (1) over one element because pred says no" << std::endl;
				++iter();
			}
			print_range();
		}

		// begin/end only -- MOVES BOTH
		selective_iterator_mixin(Iter&& begin, Iter&& end, const Pred& pred = Pred()) 
		: have_begin(true), m_begin(std::move(begin)), m_end(std::move(end)), m_pred(pred)
		{
			//std::cerr << "Two-arg moving constructor called for " << this << std::endl;
			iter() = m_begin;
			while (static_cast<Iter&>(iter()) != this->m_end
				&& !this->m_pred(static_cast<Iter&>(iter())))
			{
				//std::cerr << "Advancing (2) over one element because pred says no" << std::endl;
				++iter();
			}
			print_range();
		}
		
		
		/* Copy and move constructors.
		 * CARE. These are not as simple as they seem.
		 * We want them to be inheritable by the mixer-in.
		 * BUT what does constructor inheritance mean, in a
		 * multiple-inheritance scenario? Which constructors
		 * are called for the other bases?
		 * I think that copy and move constructors are not actually
		 * inherited, but that a defaulted version is generated
		 * which copy/move-constructs each inherited subobject
		 * separately.
		 * The upshot is that we should *not* assign to iter() here.
		 * It will get done *before* we are called.
		 * This is particularly an issue with move semantics: the
		 * iterator subobject will already have been moved out of
		 * the argument object, so we should not use that subobject
		 * in the argument (it will be zeroed).
		 * 
		 * What about copy/move-assignment operators?
		 * I *think* they are treated the same way.
		 */
		
		// copy constructor
		selective_iterator_mixin(const self& arg) 
		: have_begin(arg.have_begin), m_begin(arg.m_begin), m_end(arg.m_end),
		  m_pred(arg.m_pred)
		{
			// print_range();
		}
		
		// move constructor
		selective_iterator_mixin(self&& arg) 
		: have_begin(std::move(arg.have_begin)),
		 m_begin(std::move(arg.m_begin)),
		 m_end(std::move(arg.m_end)),
		 m_pred(arg.m_pred)
		{
			// print_range();
		}
		
		// assignment
		self& operator=(const self& arg)
		{
			//assert(arg.m_begin == this->m_begin);
			//assert(arg.m_end == this->m_end);
			this->m_begin = arg.m_begin;
			this->m_end = arg.m_end;
			this->m_pred = arg.m_pred;
			this->have_begin = arg.have_begin;
			//iter() = arg.iter(); // the mixer-in takes care of this by using default operator=
			print_range();
			return *this;
		}
		self& operator=(self&& arg) // move assignment
		{
			//assert(arg.m_begin == this->m_begin);
			//assert(arg.m_end == this->m_end);
			this->m_begin = std::move(arg.m_begin);
			this->m_end = std::move(arg.m_end);
			this->m_pred = std::move(arg.m_pred);
			this->have_begin = std::move(arg.have_begin);
			//iter() = std::move(arg.iter()); // the mixer-in takes care of this by using default operator=
			print_range();
			return *this;
		}

		typename std::iterator_traits<Iter>::reference operator*() const { return *iter(); }
		typename std::iterator_traits<Iter>::pointer operator->() const { return &*iter(); }
		self& operator++() // prefix
		{ 	do { ++iter(); } while (iter() != m_end && !m_pred(iter())); 
			return *this; }
		self  operator++(int) // postfix ++, so copying
		{ 	Iter tmp = iter(); ++*this; // re-use prefix form 
			// return tmp
			return self(m_begin, m_end, tmp, m_pred); }
		self& operator--() // prefix --
		{	// our "--" is a bit broken if we didn't copy the BEGIN iterator
			do { --iter(); } while ((!have_begin || (iter() != m_begin)) && !m_pred(iter()));
			// if we reached the beginning and pred is still false, 
			// this means undefined behaviour is okay (the user shouldn't have
			// called us) so stop at one memory location *beyond* begin
			if ((!have_begin || iter() == m_begin) && !m_pred(iter())) --iter();
			return *this; }
		self operator--(int) // postfix, so copying
		{	Iter tmp = iter(); --*this; 
			return self(m_begin, m_end, tmp, m_pred); }
		bool operator==(const self& arg) const { return iter() == arg.iter(); }
		bool operator!=(const self& arg) const { return iter() != arg.iter(); }
	};

	template <class Pred, class Iter>
	class selective_iterator 
		: public Iter
		, public selective_iterator_mixin<Pred, Iter, selective_iterator<Pred, Iter> >
	{
		//Iter m_iter;
		typedef selective_iterator<Pred, Iter> self;
		typedef selective_iterator_mixin<Pred, Iter, self> super;
	public:
		// no default constructor -- we rely on Iter being default-constructable
		using super::selective_iterator_mixin;
		using super::operator++;
		using super::operator--;
		//usine super::operator=;
		using super::operator!=;
		using super::operator==;
		using super::operator*;
		using super::operator->;
		
		//using Iter::reference;
		typedef typename Iter::reference reference;
		typedef typename Iter::difference_type difference_type;
		typedef typename Iter::value_type value_type;
		typedef typename Iter::pointer pointer;
		typedef typename Iter::iterator_category iterator_category;
	};

} // end namespace srk31

#endif
