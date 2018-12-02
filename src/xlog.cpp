#include "../include/xlog"

#include <iostream>

namespace
{
	bool colored_output = false;
	xlog::log_level min_level = xlog::all;
}

void xlog::enable_colors(bool enabled)
{
	colored_output = enabled;
}

void xlog::set_verbosity(log_level min)
{
	min_level = min;
}

xlog::log::log(xlog::log_level lvl) :
  level(lvl),
  text()
{

}

xlog::log::log(char const * prefix, xlog::log_level lvl) :
  level(lvl),
  text()
{
	text << prefix << ": ";
}

xlog::log::~log()
{
	if(level >= min_level)
	{
		auto & stream =  (level >= warning) ? std::cerr : std::cout;

		if(colored_output)
		{
			stream << "\x1b[";
			if(level >= critical)
				stream << "35"; // magenta
			else if(level >= error)
				stream << "31"; // red
			else if(level >= warning)
				stream << "33"; // yellow
			else if(level < message)
				stream << "32"; // green
			else if(level < message)
				stream << "39"; // "reset/default"
			stream << "m";
		}

		stream << text.str() << std::endl;

		if(colored_output)
		{
			stream << "\x1b[39m";
		}
	}
	if(level >= critical)
		abort();
}
