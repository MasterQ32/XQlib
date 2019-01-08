#include "../include/xio/ostream"
#include "../include/xio/istream"
#include "../include/xio/iostream"
#include "../include/xcept"

#include <sstream>
#include <gsl/gsl>

xio::ostream::ostream()
{

}

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



xio::istream::istream()
{

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

std::string xio::istream::read_line()
{
	std::stringstream stream;
	while(true)
	{
		auto const c = read<char>();
		if(c == '\r')
			continue;
		if(c == '\n')
			break;
		stream << c;
	}
	return stream.str();
}


//! Reads bytes until the end of stream is reached.
std::vector<std::byte> xio::istream::read_to_end()
{
	size_t const block_size = (1<<20); // read 1 MB blocks

	std::vector<std::byte> result;
	result.reserve(10<<20); // Reserve 10 MB
	while(true)
	{
		auto const start = result.size();
		result.resize(start + block_size);

		auto const count = read_raw(&result[start], block_size);
		if(count < 0)
			throw xcept::io_error("failed to read data.");
		else if(count == 0)
			break;
		else
			result.resize(start + gsl::narrow_cast<size_t>(count));
	}
	return result;
}


xio::iostream::~iostream()
{

}
