#ifndef SRK31_INDENTING_OSTREAM_HPP_
#define SRK31_INDENTING_OSTREAM_HPP_

#include <map>
#include <vector>
#include <stack>
#include <string>
#include <memory>
#include <iostream>
#include <cassert>
#include <unistd.h>
#include <boost/iostreams/concepts.hpp>    // input_filter
#include <boost/iostreams/operations.hpp>  // get()
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <fileno.hpp>

// namespace boost { namespace iostreams { class filtering_ostreambuf; } }

namespace srk31 {
	// this class does all the work
	struct newline_tabbing_filter;
    
    // this assumes the stream is a tty, so outputs control characters
	class indenting_tty_ostream : public std::ostream
	{
		int indent_level;
		newline_tabbing_filter *p_stream_filter;
		boost::iostreams::filtering_ostreambuf *p_wrapped_streambuf;
	public:
		indenting_tty_ostream(std::ostream& s = std::cout); 
		virtual ~indenting_tty_ostream();
		
		int inc_level() { indent_level++; *this << '\t'; return indent_level; }
		int dec_level() { indent_level--; *this << "\b\b\b\b\b\b\b\b"; return indent_level; }
		int level() { return indent_level; }

	}; 
	extern indenting_tty_ostream indenting_tty_cout, indenting_tty_cerr;

	// this doesn't assume a tty, but outputs an extra newlines on dec_level()
	class indenting_newline_ostream : public std::ostream
	{
		int indent_level;
		newline_tabbing_filter *p_stream_filter;
		boost::iostreams::filtering_ostreambuf *p_wrapped_streambuf;
	public:
		indenting_newline_ostream(std::ostream& s = std::cout); 
		virtual ~indenting_newline_ostream();
		
		int inc_level() { indent_level++; *this << "\n"; return indent_level; }
		int dec_level() { indent_level--; *this << "\n"; return indent_level; }
		int level() { return indent_level; }

	}; 
	extern indenting_newline_ostream indenting_newline_cout, indenting_newline_cerr;
    
    // this one guesses using isatty
    class indenting_ostream : public std::ostream
    {
		int indent_level;
		newline_tabbing_filter *p_stream_filter;
		boost::iostreams::filtering_ostreambuf *p_wrapped_streambuf;
	public:
		indenting_ostream(std::ostream& s = std::cout); 
		virtual ~indenting_ostream();
		
		int inc_level() { indent_level++; *this << "\n"; return indent_level; }
		int dec_level() { indent_level--; 
        	if (isatty(fileno(*this))) *this << "\b\b\b\b\b\b\b\b";
            else *this << "\n";
            return indent_level; }
		int level() { return indent_level; }
    };

	extern indenting_ostream indenting_cout;
    extern indenting_ostream indenting_cerr;
}

#endif
