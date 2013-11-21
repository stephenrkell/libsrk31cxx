template <typename Integral> 
Integral rotate_left(Integral val, unsigned n)
{
	return (val << n) | (val >> ((sizeof (Integral) * 8) - (signed) n));
} 

template <typename Integral> 
Integral rotate_right(Integral val, unsigned n)
{
	return (val >> n) | (val << ((sizeof (Integral) * 8) - (signed) n));
}
