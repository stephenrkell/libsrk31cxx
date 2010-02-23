#ifndef SRK31_INDENTING_OSTREAM_HPP_
#define SRK31_INDENTING_OSTREAM_HPP_

#include <map>
#include <vector>
#include <stack>
#include <string>
#include <memory>
#include <iostream>
#include <cassert>
#include <boost/iostreams/concepts.hpp>    // input_filter
#include <boost/iostreams/operations.hpp>  // get()
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>

//namespace boost { namespace iostreams { class filtering_ostreambuf; } }

namespace srk31 {
	struct newline_tabbing_filter;
	class indenting_ostream : public std::ostream
	{
		int indent_level;
		newline_tabbing_filter *p_stream_filter;
		boost::iostreams::filtering_ostreambuf *p_wrapped_streambuf;
	public:
		indenting_ostream(std::ostream& s = std::cout); 
		virtual ~indenting_ostream();
		
		int inc_level() { indent_level++; *this << '\t'; return indent_level; }
		int dec_level() { indent_level--; *this << "\b\b\b\b\b\b\b\b"; return indent_level; }
		int level() { return indent_level; }

	}; 

	extern indenting_ostream indenting_cout, indenting_cerr;
}

#endif
