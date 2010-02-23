#ifndef SRK31_DOWNCASTING_ITERATOR_HPP_
#define SRK31_DOWNCASTING_ITERATOR_HPP_

#include <iterator>

template<typename Iter, typename DownTo>
class downcasting_iterator
    : public std::iterator<	typename std::iterator_traits<Iter>::iterator_category,
    					DownTo*, // value type
                        typename std::ptrdiff_t, // difference type
                        DownTo**, // pointer type
                        DownTo*& > // reference type
{
    Iter m_iter;
    typedef downcasting_iterator<Iter, DownTo> self;
public:
    // default constructor
    downcasting_iterator(Iter iter) : m_iter(iter) {}

	// HACK: we need to return by value because of iterator_with_lens's brokeness
    DownTo* operator*() const 
    //{ return *wrap_die(*(*m_iter)); }
    { return         dynamic_cast<DownTo*>(*m_iter);
      /*return *(this->operator->()); */ }
    DownTo** operator->() const 
    //{ return wrap_die(*(*m_iter)); }
    { return         &(dynamic_cast<DownTo*>(*m_iter)); }

    self& operator++() // prefix
    { 	m_iter++; return *this; }
    self  operator++(int) // postfix ++
    { 	self tmp = m_iter; ++m_iter; // re-use prefix form 
        return tmp; }
    self& operator--() // prefix --
    {	m_iter--; return *this; }
    self operator--(int) // postfix
    { 	self tmp = m_iter; --m_iter; 
    	return tmp; }
    bool operator==(const self& arg) const { return this->m_iter == arg.m_iter; }
    bool operator!=(const self& arg) const { return this->m_iter != arg.m_iter; }
};

#endif
