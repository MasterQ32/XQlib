#include "../include/xio/compression/zstd"

#include <zstd.h>
#include <xstd/resource>
#include <cassert>
#include <xcept>

int xio::compression::zstd::get_max_compression_level()
{
	return ZSTD_maxCLevel();
}

//! Compresses a zstd data stream and returns the number of compressed bytes.
size_t xio::compression::zstd::compress(istream & in, ostream & out, int compressionLevel)
{
	// reuse resources whenever possible (buffers as well as encoder)
	xstd::resource<ZSTD_CStream, ZSTD_freeCStream> static thread_local encoder(ZSTD_createCStream());
	std::vector<std::byte> static thread_local src_buffer(ZSTD_CStreamInSize());
	std::vector<std::byte> static thread_local dst_buffer(ZSTD_CStreamOutSize());

	assert(encoder);

	if(auto const error = ZSTD_initCStream(encoder.get(), compressionLevel); ZSTD_isError(error))
	{
		throw xcept::io_error(ZSTD_getErrorName(error));
	}

	size_t total_bytes_written = 0;
  size_t read;
	size_t toRead = src_buffer.size();
  while( (read = in.read_raw(src_buffer.data(), toRead)) > 0 )
	{
      ZSTD_inBuffer input = { src_buffer.data(), read, 0 };
      while (input.pos < input.size)
			{
          ZSTD_outBuffer output = { dst_buffer.data(), dst_buffer.size(), 0 };
          toRead = ZSTD_compressStream(encoder.get(), &output , &input);   /* toRead is guaranteed to be <= ZSTD_CStreamInSize() */
          if (ZSTD_isError(toRead))
					{
						throw xcept::io_error(ZSTD_getErrorName(toRead));
          }
          if (toRead > src_buffer.size())
						toRead = src_buffer.size();   /* Safely handle case when `buffInSize` is manually changed to a value < ZSTD_CStreamInSize()*/
					out.write(dst_buffer.data(), output.pos);
					total_bytes_written += output.pos;
      }
  }

  ZSTD_outBuffer output = { dst_buffer.data(), dst_buffer.size(), 0 };
  size_t const remainingToFlush = ZSTD_endStream(encoder.get(), &output);   /* close frame */
  if (remainingToFlush > 0)
		throw xcept::io_error("not fully flushed");
  out.write(dst_buffer.data(), output.pos);
	total_bytes_written += output.pos;

	return total_bytes_written;
}

//! Decompresses a zstd data stream and returns the number of decompressed bytes.
size_t xio::compression::zstd::decompress(istream & in, ostream & out)
{
	// reuse resources whenever possible (buffers as well as decoder)
	xstd::resource<ZSTD_DStream, ZSTD_freeDStream> static thread_local decoder(ZSTD_createDStream());
	std::vector<std::byte> static thread_local src_buffer(ZSTD_DStreamInSize());
	std::vector<std::byte> static thread_local dst_buffer(ZSTD_DStreamOutSize());

	assert(decoder);

	/* In more complex scenarios, a file may consist of multiple appended frames (ex : pzstd).
  *  The following example decompresses only the first frame.
  *  It is compatible with other provided streaming examples */
	size_t toRead = ZSTD_initDStream(decoder.get());
	if(ZSTD_isError(toRead))
	{
		throw xcept::io_error(ZSTD_getErrorName(toRead));
	}

  size_t read;
	size_t total_bytes_written = 0;
  while ( (read = in.read_raw(src_buffer.data(), toRead)) > 0 )
	{
      ZSTD_inBuffer input = { src_buffer.data(), read, 0 };
      while (input.pos < input.size)
			{
          ZSTD_outBuffer output = { dst_buffer.data(), dst_buffer.size(), 0 };
          toRead = ZSTD_decompressStream(decoder.get(), &output , &input);  /* toRead : size of next compressed block */
          if (ZSTD_isError(toRead))
						throw xcept::io_error(ZSTD_getErrorName(toRead));
          out.write(dst_buffer.data(), output.pos);
					total_bytes_written += output.pos;
      }
  }
	return total_bytes_written;
}
