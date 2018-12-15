#include "../include/xio/ostream"
#include "../include/xio/istream"
#include "../include/xio/iostream"
#include "../include/xcept"

#include <sstream>
#include <gsl/gsl>

xio::ostream::~ostream()
{

}

void xio::ostream::write(void const * data, size_t length)
{
	size_t offset = 0;
	while(offset < length)
	{
		auto const count = write_raw(reinterpret_cast<std::byte const *>(data) + offset, length - offset);
		if(count < 0)
			throw xcept::io_error("failed to send data.");
		else if(count == 0)
			throw xcept::end_of_stream("reached end of stream before all data could be written!");
		else
			offset += gsl::narrow_cast<size_t>(count);
	}
}



xio::istream::~istream()
{

}

void xio::istream::read(void * data, size_t length)
{
	size_t offset = 0;
	while(offset < length)
	{
		auto const count = read_raw(reinterpret_cast<std::byte *>(data) + offset, length - offset);
		if(count < 0)
			throw xcept::io_error("failed to send data.");
		else if(count == 0)
			throw xcept::end_of_stream("reached end of stream before all data could be written!");
		else
			offset += gsl::narrow_cast<size_t>(count);
	}
}

std::string xio::istream::read_line(char const * delimiter, char const * ignore)
{
	std::stringstream stream;
	if(delimiter == nullptr) delimiter = "";
	if(ignore == nullptr)    ignore = "";
	while(true)
	{
		auto const c = read<char>();
		if(strchr(ignore, c) != nullptr)
			continue;
		if(strchr(delimiter, c) != nullptr)
			break;
		stream << c;
	}
	return stream.str();
}

xio::iostream::~iostream()
{

}
