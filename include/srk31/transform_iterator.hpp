#ifndef SRK31_TRANSFORM_ITERATOR_HPP_
#define SRK31_TRANSFORM_ITERATOR_HPP_

#include <iterator>
#include <iostream>
#include <cassert>
#include <srk31/util.hpp>

/* Most of the time, you should use boost::transform_iterator instead. 
 * But this is factored as a a mixin, so that you can make iterators 
 * which inherit from whatever the base iterator type is.
 * 
 * See notes in selective_iterator.hpp.
 */

namespace srk31
{
	template <class Func, class Iter>
	class transform_iterator;
	
	template <class Func, class Iter, class MixerIn>
	class transform_iterator_mixin
	{
		typedef transform_iterator_mixin<Func, Iter, MixerIn> self;
		typedef MixerIn super;
	public:
		Func m_func;

		const Iter& iter() const { return *static_cast<const MixerIn *>(this); }
		      Iter& iter()       { return *static_cast<      MixerIn *>(this); }

		// fully specifying constructor
		transform_iterator_mixin(Iter&& i, const Func& func = Func())
		: m_func(func)
		{}
		
		// fully specifying constructor
		transform_iterator_mixin(const Iter& i, const Func& func = Func())
		: m_func(func)
		{}
		
		// default constructor
		transform_iterator_mixin() {}
		
		// copy constructor
		transform_iterator_mixin(const self& arg)
		: m_func(arg.m_func)
		{}
		
		// move constructor
		transform_iterator_mixin(self&& arg)
		: m_func(std::move(arg.m_func))
		{}
		
		// assignment
		self& operator=(const self& arg)
		{
			assert(&this->m_func == &arg.m_func);
			return *this;
		}
		self& operator=(self&& arg) // move assignment
		{
			assert(&this->m_func == &arg.m_func);
			return *this;
		}

		typedef typename Func::result_type reference;
		typedef typename std::remove_reference<typename Func::result_type>::type value_type;
		typedef value_type *pointer;
		typedef typename Iter::difference_type difference_type;
		typedef typename Iter::iterator_category iterator_category;

private:
		void print_m_func() const
		{
// 			typedef typename
// 				std::tuple_element<0, typename function_traits<Func>::argument_types>::type
// 				first_argument_type;
// 			std::cerr << "Derefing via function (with target? "
// 				<< std::boolalpha << m_func.operator bool()
// 				<< ") "
// 				<< (void*) m_func.template target<reference(first_argument_type)>()
// 				<< std::endl;
		}
public:
		reference operator*() const
		{
			this->print_m_func();
			return m_func(*iter());
		}
		pointer operator->() const 
		{
			this->print_m_func();
			return &(m_func(*iter()));
		}
		self& operator++() // prefix
		{
			++iter();
			return *this;
		}
		self operator++(int) // postfix ++, so copying
		{
			Iter tmp = iter();
			++tmp;
			return self(std::move(tmp), m_func);
		}
		self& operator--() // prefix --
		{
			--iter();
			return *this;
		}
		self operator--(int) // postfix, so copying
		{
			Iter tmp = iter();
			--tmp;
			return self(std::move(tmp), m_func);
		}
		bool operator==(const self& arg) const { return iter() == arg.iter(); }
		bool operator!=(const self& arg) const { return iter() != arg.iter(); }
	};

	template <class Func, class Iter>
	class transform_iterator 
		: public Iter
		, public transform_iterator_mixin<Func, Iter, transform_iterator<Func, Iter> >
	{
		typedef transform_iterator<Func, Iter> self;
		typedef transform_iterator_mixin<Func, Iter, self> super;
	public:
		/* We don't just forward the mixin's constructors, because 
		 * sometimes Iter will not have a default constructor -- rather,
		 * we have to pass our arguments to both bases. The mixin ignores
		 * any iterator argument, so it's up to Iter to initialize itself
		 * using this. FIXME: probably want to make selective_iterator
		 * consistent with this. */
		transform_iterator(Iter&& i, const Func& func = Func())
		: Iter(std::move(i)), super(i, func) {}
		
		// fully specifying constructor
		transform_iterator(const Iter& i, const Func& func = Func())
		: Iter(i), super(i, func) {}
		
		/* In cases where we can default-construct m_func,
		 * we can offer the Iter's constuctors too. */
		using Iter::Iter;
		
		typedef typename Func::result_type reference;
		typedef typename std::remove_reference<typename Func::result_type>::type value_type;
		typedef value_type *pointer;
		typedef typename Iter::difference_type difference_type;
		typedef typename Iter::iterator_category iterator_category;
		
		using super::operator++;
		using super::operator--;
		using super::operator!=;
		using super::operator==;
		using super::operator*;
		using super::operator->;
	};

} // end namespace srk31

#endif
