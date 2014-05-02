#ifndef SRK31CXX_ENDIAN_HPP_
#define SRK31CXX_ENDIAN_HPP_

namespace srk31
{

// forward declarations
inline bool host_is_big_endian();
inline bool host_is_little_endian();

typedef char intsize_char_array[sizeof (int)];

// forward decls
union int_and_chars;

#define is_big_endian_macro    (one_in_msbyte.chars[0] == 1)
#define is_little_endian_macro (one_in_lsbyte.chars[0] == 1)

union int_and_chars
{
	int i;
	intsize_char_array chars;
};
const union int_and_chars one_in_lsbyte = { .i = 1 };
const union int_and_chars one_in_msbyte = { .i = 1 << (sizeof (int) - 1) };

/* Alas, these don't work. */
// #define one_in_lsbyte ((int_and_chars){ 1 })
// #define one_in_msbyte ((int_and_chars){ 1 << (sizeof (int) - 1) })

namespace
{	
	struct endianness_checks
	{
		// alas, these don't work
		// static_assert(is_little_endian_macro || is_big_endian_macro, "endianness logic error");
		// static_assert(!(is_little_endian_macro && is_big_endian_macro), "endianness logic error");
	};
}

inline bool host_is_big_endian()
{
	return is_big_endian_macro;
}
inline bool host_is_little_endian()
{
	return is_little_endian_macro;
}

} // end namespace srk31

// #undef one_in_lsbyte
// #undef one_in_msbyte
#undef is_big_endian_macro
#undef is_little_endian_macro

#endif
