#ifndef SRK31_ITERATOR_WITH_LENS_HPP_
#define SRK31_ITERATOR_WITH_LENS_HPP_

#include <iterator>
#include <functional>

template<typename O, typename I>
struct Lens
{
	typedef O ObjectType;
    typedef I ImageType;
	typedef std::unary_function<ObjectType, ImageType> get_type;
    get_type get; // hide this to specialise
    typedef std::unary_function<ImageType, ObjectType> putback_type;
    putback_type putback; // hide this to specialise
};

template<typename Iter, typename Lens>
class iterator_with_lens
    : public std::iterator<	typename std::iterator_traits<Iter>::iterator_category,
    					typename Lens::ImageType, // value type
                        typename std::ptrdiff_t, // difference type
                        typename Lens::ImageType*, // pointer type
                        typename Lens::ImageType& > // reference type
{
    Iter m_iter;
    Lens m_lens;
    typedef iterator_with_lens<Iter, Lens> self;
public:
    // default constructor
    iterator_with_lens() : m_iter(), m_lens(Lens()) {}
    
    explicit iterator_with_lens(Iter iter, const Lens& lens = Lens())
     :	m_iter(iter), m_lens(lens) {}
    
    explicit iterator_with_lens(Iter iter) : m_iter(iter) {}

	// FIXME: this is okay as long as you don't try to write through the lens!
    typename Lens::ImageType operator*() const 
    //{ return *wrap_die(*(*m_iter)); }
    { return         m_lens.get(*m_iter);
      /*return *(this->operator->()); */ }
    typename Lens::ImageType* operator->() const 
    //{ return wrap_die(*(*m_iter)); }
    { return         &(m_lens.get(*m_iter)); }

    self& operator++() // prefix
    { 	m_iter++; return *this; }
    self  operator++(int) // postfix ++
    { 	Iter tmp = m_iter; ++m_iter; // re-use prefix form 
        return iterator_with_lens(tmp, m_lens); }
    self& operator--() // prefix --
    {	m_iter--; return *this; }
    self operator--(int) // postfix
    { 	Iter tmp = m_iter; --m_iter; 
    	return iterator_with_lens(tmp, m_lens); }
    bool operator==(const self& arg) const { return this->m_iter == arg.m_iter; }
    bool operator!=(const self& arg) const { return this->m_iter != arg.m_iter; }
};

#endif
