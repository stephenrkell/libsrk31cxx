#ifndef SRK31_SELECTIVE_ITERATOR_HPP_
#define SRK31_SELECTIVE_ITERATOR_HPP_

#include <iterator>
#include <iostream>
#include <cassert>

template <class Iter, class Pred>
class selective_iterator 
    : public std::iterator_traits<Iter> // borrow container's typedefs
{
	const Iter m_begin;
    const Iter m_end;
    const Pred& m_pred;
    Iter m_iter;
    typedef selective_iterator<Iter, Pred> self;
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
    selective_iterator(Iter begin, Iter end, Iter val, Pred pred = Pred()) 
    : m_begin(begin), m_end(end), m_pred(pred), m_iter(val)
    { 	assert(m_iter == m_end || m_pred(m_iter)); /*print_range();*/ }
    explicit selective_iterator(Iter begin, Iter end, Pred pred = Pred()) 
    : m_begin(begin), m_end(end), m_pred(pred), m_iter(begin)
    { 	while (m_iter != m_end && !m_pred(m_iter)) m_iter++; /*print_range();*/ }
    // assignment
    selective_iterator<Iter, Pred>& operator=(const selective_iterator<Iter, Pred>& arg)
    {
    	assert(arg.m_begin == this->m_begin);
        assert(arg.m_end == this->m_end);
        assert(arg.m_pred == this->m_pred);
        m_iter = arg.m_iter;
        return *this;
    }
    // copy constructor
    selective_iterator(const selective_iterator<Iter, Pred>& arg) 
    : m_begin(arg.m_begin), m_end(arg.m_end), m_pred(arg.m_pred), m_iter(arg.m_iter) {}

    typename /*std::iterator_traits<Iter>::reference*/ Iter::value_type operator*() const { return *m_iter; }
    typename std::iterator_traits<Iter>::pointer operator->() const { return &*m_iter; }
    self& operator++() // prefix
    { 	do { m_iter++; } while (m_iter != m_end && !m_pred(m_iter)); 
        return *this; }
    self  operator++(int) // postfix ++
    { 	Iter tmp = m_iter; ++*this; // re-use prefix form 
    	// return tmp
        return self(m_begin, m_end, tmp, m_pred); }
    self& operator--() // prefix --
    {	do { m_iter--; } while (m_iter != m_begin && !m_pred(m_iter));
        // if we reached the beginning and pred is still false, 
        // this means undefined behaviour is okay (the user shouldn't have
        // called us) so stop at one memory location *beyond* begin
        if (m_iter == m_begin && !m_pred(m_iter)) m_iter--;
        return *this; }
    self operator--(int) // postfix
    { 	Iter tmp = m_iter; --*this; 
    	//return tmp; 
        return self(m_begin, m_end, tmp, m_pred); }
    bool operator==(const self& arg) const { return this->m_iter == arg.m_iter; }
    bool operator!=(const self& arg) const { return this->m_iter != arg.m_iter; }
};

#endif
