#include "../include/xio/compression/lzma"

#include <cerrno>
#include <cstring>
#include <lzma.h>
#include <xcept>
#include <gsl/gsl>

namespace
{
	struct lzma_helper
	{
		static lzma_ret validate(lzma_ret ret)
		{
			// Something went wrong. The possible errors are documented in
			// lzma/container.h (src/liblzma/api/lzma/container.h in the source
			// package or e.g. /usr/include/lzma/container.h depending on the
			// install prefix).
			switch( ret )
			{
				case LZMA_OK:
				case LZMA_STREAM_END:
				case LZMA_NO_CHECK:
					return ret;

				case LZMA_MEM_ERROR:
					throw xcept::invalid_operation("Memory allocation failed");

				case LZMA_OPTIONS_ERROR:
					throw xcept::invalid_operation("Specified preset is not supported");

				case LZMA_UNSUPPORTED_CHECK:
					throw xcept::invalid_operation("Specified integrity check is not supported");

        case LZMA_DATA_ERROR:
					// This error is returned if the compressed
					// or uncompressed size get near 8 EiB
					// (2^63 bytes) because that's where the .xz
					// file format size limits currently are.
					// That is, the possibility of this error
					// is mostly theoretical unless you are doing
					// something very unusual.
					//
					// Note that strm->total_in and strm->total_out
					// have nothing to do with this error. Changing
					// those variables won't increase or decrease
					// the chance of getting this error.
					throw xcept::invalid_operation("File size limits exceeded");

				default:
					break;
			}
			// This is most likely LZMA_PROG_ERROR indicating a bug in
			// this program or in liblzma. It is inconvenient to have a
			// separate error message for errors that should be impossible
			// to occur, but knowing the error code is important for
			// debugging. That's why it is good to print the error code
			// at least when there is no good error message to show.
			throw xcept::invalid_operation("Unknown error, possibly a bug");
		}

		enum mode
		{
			compressor,
			decompressor
		};

		lzma_stream stream;
		explicit lzma_helper(mode mode, uint32_t preset_or_flags) : stream( LZMA_STREAM_INIT )
		{
			// Initialize the encoder using a preset. Set the integrity to check
	    // to CRC64, which is the default in the xz command line tool. If
	    // the .xz file needs to be decompressed with XZ Embedded, use
	    // LZMA_CHECK_CRC32 instead.
			if(mode == compressor)
				validate(lzma_easy_encoder( &stream, preset_or_flags, LZMA_CHECK_CRC64 ));
			else
				validate(lzma_stream_decoder(&stream, UINT64_MAX, preset_or_flags));
		}

		lzma_helper(lzma_helper const &) = delete;
		lzma_helper(lzma_helper &&) = delete;
		~lzma_helper() {
				lzma_end( &stream );
		}

		operator lzma_stream *       ()       { return &stream; }
		/// operator lzma_stream const * () const { return &stream; }

		lzma_stream *       operator -> ()       { return &stream; }
		/// lzma_stream const * operator -> () const { return &stream; }
	};
}

static size_t do_compression(lzma_helper & strm, xio::istream& istream, xio::ostream& ostream)
{
	size_t constexpr buffer_size = (1 << 20);

	lzma_action action = LZMA_RUN;

	static thread_local std::array<std::byte, buffer_size> inbuf;
	static thread_local std::array<std::byte, buffer_size> outbuf;

	strm->next_in = nullptr;
	strm->avail_in = 0;
	strm->next_out = reinterpret_cast<uint8_t *>(outbuf.data());
	strm->avail_out = sizeof( outbuf );

	size_t total_transfer = 0;
	while( true )
	{
		// Fill the input buffer if it is empty.
		if(strm->avail_in == 0)
		{
			strm->next_in = reinterpret_cast<uint8_t const *>(inbuf.data());
			strm->avail_in = gsl::narrow<size_t>(istream.read_raw(inbuf.data(), inbuf.size()));

			if(strm->avail_in == 0)
				action = LZMA_FINISH;
		}

		auto const ret = lzma_helper::validate(lzma_code( strm, action ));

		if( (strm->avail_out == 0) or (ret == LZMA_STREAM_END) )
		{
			// When lzma_code() has returned LZMA_STREAM_END,
			// the output buffer is likely to be only partially
			// full. Calculate how much new data there is to
			// be written to the output file.
			size_t write_size = outbuf.size() - strm->avail_out;

			ostream.write(outbuf.data(), write_size);

			total_transfer += write_size;

			// Reset next_out and avail_out.
			strm->next_out = reinterpret_cast<uint8_t *>(outbuf.data());
			strm->avail_out = outbuf.size();
		}
		if(ret == LZMA_STREAM_END)
			return total_transfer;
	}
}

size_t xio::compression::lzma::compress(xio::istream& istream, xio::ostream& ostream, uint32_t preset)
{
	lzma_helper strm(lzma_helper::compressor, preset);

	return do_compression(strm, istream, ostream);
}

size_t xio::compression::lzma::decompress(xio::istream& istream, xio::ostream& ostream)
{
	lzma_helper strm(lzma_helper::decompressor, LZMA_CONCATENATED);

	return do_compression(strm, istream, ostream);
}
