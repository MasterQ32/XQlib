#include "../include/xio/ostream"
#include "../include/xio/istream"
#include "../include/xio/iostream"
#include "../include/xio/memory_stream"
#include "../include/xio/file_stream"
#include "../include/xio/utility"
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
		if(count == 0)
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
		if(count == 0)
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
		if(count == 0)
			break;
		else
			result.resize(start + gsl::narrow_cast<size_t>(count));
	}
	return result;
}


xio::iostream::~iostream()
{

}


xio::memory_stream::memory_stream() :
  buffer(),
  cursor(0)
{

}

xio::memory_stream::memory_stream(size_t initial_size) :
  buffer(initial_size),
  cursor(0)
{

}

xio::memory_stream::memory_stream(void * const initial_data, size_t size) :
  buffer(size),
  cursor(0)
{
	assert(buffer.size() == size);
	memcpy(buffer.data(), initial_data, size);
}

size_t xio::memory_stream::read_raw(void * data, size_t length) noexcept
{
	size_t const top = std::min(cursor + length, buffer.size());
	size_t const actual_length = top - cursor;
	memcpy(data, buffer.data() + cursor, actual_length);
	cursor += actual_length;
	return actual_length;
}

size_t xio::memory_stream::write_raw(void const * data, size_t length) noexcept
{
	buffer.resize(std::max(buffer.size(), cursor + length));
	memcpy(buffer.data() + cursor, data, length);
	cursor += length;
	return length;
}

xio::file_stream::file_stream(char const * fileName, char const * mode) :
  handle(fopen(fileName, mode))
{
	if(not handle)
		throw xcept::io_error("Could not open file!");
}

size_t xio::file_stream::read_raw(void * data, size_t length) noexcept
{
	return fread(data, 1, length, handle.get());
}

size_t xio::file_stream::write_raw(void const * data, size_t length) noexcept
{
	return fwrite(data, 1, length, handle.get());
}

size_t xio::file_stream::tell() const
{
	return gsl::narrow<size_t>(ftell(const_cast<FILE*>(handle.get())));
}

void xio::file_stream::seek(ssize_t offset, int mode)
{
	if(fseek(handle.get(), offset, mode) != 0)
		throw xcept::io_error("failed to seek.");
}

size_t static const constexpr copy_block_size = 1 << 18;

void xio::copy(istream & source, ostream & target)
{
	std::array<std::byte, copy_block_size> buffer;
	while(true)
	{
		auto const count = source.read_raw(buffer.data(), buffer.size());
		if(count == 0)
			return;
		target.write(buffer.data(), count);
	}
}

void xio::copy(istream & source, ostream & target, size_t num_bytes)
{
	std::array<std::byte, copy_block_size> buffer;
	while(num_bytes > 0)
	{
		auto const remaining = std::min(num_bytes, buffer.size());
		auto const count = source.read_raw(buffer.data(), remaining);
		if(count == 0)
			return;
		target.write(buffer.data(), count);
		num_bytes -= count;
	}
}
