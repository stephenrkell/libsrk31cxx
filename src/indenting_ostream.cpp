#include "indenting_ostream.hpp"

namespace srk31
{
	struct newline_tabbing_filter : boost::iostreams::output_filter 
	{
		int *indent_level;
		newline_tabbing_filter(int *plevel) : indent_level(plevel) {}
		template<typename Sink>
		bool put_char(Sink& dest, int c)
		{
			if (!boost::iostreams::put(dest, c)) return false;
			if (c == '\n')
			{
				for (int i = *indent_level; i > 0; i--) 
				{
					if (!boost::iostreams::put(dest, '\t')) return false;
				}
			}
			return true;
		}
		template<typename Sink>
    	bool put(Sink& dest, int c) 
    	{
        	return put_char(dest, c);
    	}
	};
	
	indenting_tty_ostream::indenting_tty_ostream(std::ostream& s) : 
		std::ostream(p_wrapped_streambuf = new boost::iostreams::filtering_ostreambuf),
		indent_level(0),
		p_stream_filter(new newline_tabbing_filter(&indent_level))
	{
		p_wrapped_streambuf->push(*p_stream_filter);
		p_wrapped_streambuf->push(s);
	}
	indenting_tty_ostream::~indenting_tty_ostream() { delete p_stream_filter; delete p_wrapped_streambuf; }
	indenting_tty_ostream indenting_tty_cout(std::cout), indenting_tty_cerr(std::cerr);

	indenting_newline_ostream::indenting_newline_ostream(std::ostream& s) : 
		std::ostream(p_wrapped_streambuf = new boost::iostreams::filtering_ostreambuf),
		indent_level(0),
		p_stream_filter(new newline_tabbing_filter(&indent_level))
	{
		p_wrapped_streambuf->push(*p_stream_filter);
		p_wrapped_streambuf->push(s);
	}
	indenting_newline_ostream::~indenting_newline_ostream() { delete p_stream_filter; delete p_wrapped_streambuf; }
	indenting_newline_ostream indenting_newline_cout(std::cout), indenting_newline_cerr(std::cerr);

	indenting_ostream::indenting_ostream(std::ostream& s) : 
		std::ostream(p_wrapped_streambuf = new boost::iostreams::filtering_ostreambuf),
		indent_level(0),
		p_stream_filter(new newline_tabbing_filter(&indent_level))
	{
		p_wrapped_streambuf->push(*p_stream_filter);
		p_wrapped_streambuf->push(s);
	}
	indenting_ostream::~indenting_ostream() { delete p_stream_filter; delete p_wrapped_streambuf; }
	indenting_ostream indenting_cout(std::cout), indenting_cerr(std::cerr);


}
