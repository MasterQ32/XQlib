#include "../include/xio/ostream"
#include "../include/xio/istream"
#include "../include/xio/iostream"
#include "../include/xio/memory_stream"
#include "../include/xio/file_stream"
#include "../include/xio/seekable_stream"
#include "../include/xio/utility"
#include "../include/xio/simple"
#include "../include/xcept"

#include <sstream>
#include <gsl/gsl>

xio::seekable_stream::~seekable_stream() noexcept
{

}

xio::ostream::ostream() noexcept
{

}

xio::ostream::~ostream() noexcept
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



xio::istream::istream() noexcept
{

}

xio::istream::~istream() noexcept
{

}

void xio::istream::read(void * data, size_t length)
{
	size_t offset = 0;
	while(offset < length)
	{
		auto const count = read_raw(reinterpret_cast<std::byte *>(data) + offset, length - offset);
		if(count == 0)
			throw xcept::end_of_stream("reached end of stream before all data could be read!");
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

xio::memory_stream::memory_stream(memory_stream && other) :
  buffer(std::move(other.buffer)),
  cursor(other.cursor)
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



void xio::memory_stream::rewind()
{
	cursor = 0;
}

size_t xio::memory_stream::tell() const
{
	return cursor;
}

size_t xio::memory_stream::seek(ssize_t offset, seek_type type)
{
	ssize_t new_cursor = gsl::narrow<ssize_t>(cursor);
	auto const length = gsl::narrow<ssize_t>(buffer.size());
	switch(type)
	{
		case set:
			new_cursor = offset;
			break;
		case cur:
			new_cursor += offset;
			break;
		case end:
			new_cursor = length + offset;
			break;
	}
	cursor = gsl::narrow<size_t>(std::clamp<ssize_t>(new_cursor, 0, length));
	return cursor;
}

xio::file_stream::file_stream(char const * fileName, char const * mode) :
  handle(fopen(fileName, mode))
{
	if(not handle)
		throw xcept::io_error("Could not open file!");
}

xio::file_stream::file_stream(file_stream && other) :
  handle(std::move(other.handle))
{

}

size_t xio::file_stream::read_raw(void * data, size_t length) noexcept
{
	return fread(data, 1, length, handle.get());
}

size_t xio::file_stream::write_raw(void const * data, size_t length) noexcept
{
	return fwrite(data, 1, length, handle.get());
}

void xio::file_stream::flush()
{
	fflush(handle.get());
}

void xio::file_stream::rewind()
{
	auto const pos = seek(0, set);
	assert(pos == 0);
}

size_t xio::file_stream::tell() const
{
	return gsl::narrow<size_t>(ftell(const_cast<FILE*>(handle.get())));
}

size_t xio::file_stream::seek(ssize_t offset, seek_type type)
{
	if(fseek(handle.get(), offset, static_cast<int>(type)) != 0)
		throw xcept::io_error("failed to seek.");
	return tell();
}

size_t static const constexpr copy_block_size = 1 << 18;

size_t xio::copy(istream & source, ostream & target)
{
	std::array<std::byte, copy_block_size> buffer;
	size_t total = 0;
	while(true)
	{
		auto const count = source.read_raw(buffer.data(), buffer.size());
		if(count == 0)
			return total;
		target.write(buffer.data(), count);
		total += count;
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
			throw xcept::io_error("failed to copy total number of bytes.");
		target.write(buffer.data(), count);
		num_bytes -= count;
	}
}


// simple api:

std::string xio::load_string(std::filesystem::path const & path)
{
	auto const data = load_raw(path);
	return std::string(reinterpret_cast<char const *>(data.data()), data.size());
}

std::vector<std::byte> xio::load_raw(std::filesystem::path const & path)
{
	xio::file_stream stream(path, "rb");
	return stream.read_to_end();
}

void xio::save_string(std::filesystem::path const & path, std::string const & value)
{
	save_raw(path, reinterpret_cast<std::byte const *>(value.c_str()), value.size());
}

void xio::save_raw(std::filesystem::path const & path, std::byte const * value, size_t length)
{
	xio::file_stream stream(path, "wb");
	stream.write(value, length);
	stream.flush();
}
