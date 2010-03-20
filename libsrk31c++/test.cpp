#include "conjoining_iterator.hpp"
#include <vector>
#include <iostream>

template <class Iter>
void print_all(Iter begin,
	Iter end)
{
	std::cout << "[ ";
	for (Iter i = begin; i != end; i++)
    {
    	if (i != begin) std::cout << ", ";
    	std::cout << *i;
    }
    std::cout << "]" << std::endl;
}

int main()
{
	std::vector<int> first;
    std::vector<int> second;
    
    first.push_back(1); first.push_back(2); first.push_back(3);
    second.push_back(4); second.push_back(5); second.push_back(6);
    
    // test default constructor
    conjoining_sequence<std::vector<int>::iterator> a;
    a.append(first.begin(), first.end());
    a.append(second.begin(), second.end());
    print_all(a.begin(), a.end());
    
    // test one-set-arg constructor
    conjoining_sequence<std::vector<int>::iterator> b(first.begin(), first.end());
    b.append(second.begin(), second.end());
	print_all(b.begin(), b.end());
    
    // test two-set-arg constructor
    conjoining_sequence<std::vector<int>::iterator> c(first.begin(), first.end(),
    	second.begin(), second.end());
    print_all(c.begin(), c.end());
    
    // test one-empty-list behaviour
    conjoining_sequence<std::vector<int>::iterator> x(first.begin(), first.end(),
    	second.end(), second.end());
    print_all(x.begin(), x.end());
    
    // test many-empty-lists-only behaviour
    conjoining_sequence<std::vector<int>::iterator> y(first.begin(), first.begin(),
    	second.end(), second.end());
    print_all(y.begin(), y.end());
    
    
    return 0;
}
