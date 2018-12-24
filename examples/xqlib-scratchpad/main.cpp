#include <xgl/all>
#include <xapp>
#include <xlog>
#include <xgraphics/sprite_batch>
#include <xgraphics/texture_loader>
#include <xgraphics/debug_draw>
#include <random>
#include <xcs>
#include <xinput/xinput>
#include <xinput/sdl>

#include <unordered_map>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <xnet/http/client>
#include <xnet/http/server>
#include <xnet/socket>
#include <xnet/dns>
#include <xnet/ip>
#include <xcept>
#include <xstd/dynamic_array>

#include <iostream>
#include <optional>
#include <functional>

int main()
{
	xapp::options xapp_options;
	xapp_options.resizable = true;

	if(not xapp::init(xapp::opengl, xapp_options))
		return 1;

	xgraphics::debug_draw debug;

	while(xapp::update())
	{
		using glm::vec3;

		xapp::clear();

		auto const right = xapp::width() - 1;
		auto const top = xapp::height() - 1;

		debug.line(
			vec3(0, 0, 0),
			vec3(right, 0, 0));
		debug.line(
			vec3(right, 0, 0),
			vec3(right, top, 0));
		debug.line(
			vec3(right, top, 0),
			vec3(0, top, 0));
		debug.line(
			vec3(0, top, 0),
			vec3(0, 0, 0));

		debug.draw(glm::ortho(
				-0.5f,
				float(xapp::width() - 0.5),
				float(xapp::height() - 0.5),
				-0.5f));


		xapp::present();
	}
	return 0;
}

/*
namespace xsnd
{
	using stream_id = xstd::unique_id<struct stream_tag>;

	using  mono_sample = float;

	struct stereo_sample
	{
		float left;
		float right;

		stereo_sample(float v) : left(v), right(v) { }
		stereo_sample(float l, float r) : left(l), right(r) { }
	};

#define STEREO_OP(_Op) \
	inline stereo_sample operator _Op (stereo_sample const & lhs, stereo_sample const & rhs) \
	{ \
		return stereo_sample { lhs.left _Op rhs.left, lhs.right _Op rhs.right }; \
	}
	STEREO_OP(+)
	STEREO_OP(-)
	STEREO_OP(*)
	STEREO_OP(/)
#undef STEREO_OP

	template<typename T>
	using audio_source = std::function<std::size_t(T * data, std::size_t offset, std::size_t count)>;

	template<typename T>
	struct channel
	{
		std::optional<audio_source<T>> source;
		unsigned long sequence_id;
		std::size_t offset;
		float volume;
		float pitch;
		float pan;
	};

	template<typename T>
	struct mixer
	{
		xstd::dynamic_array<channel<T>> channels;
		float master_volume;

		explicit mixer(std::size_t num_channels) :
		  channels(num_channels),
		  master_volume(1.0f)
		{

		}
	};

	template<typename T>
	using pcm_data = xstd::dynamic_array<T>;

	template<typename T>
	audio_source<T> make_source(pcm_data<T> const & pcm)
	{
		return [=](T * data, std::size_t offset, std::size_t count) -> std::size_t
		{
			auto const upper_limit = std::min(offset + count, pcm.size());
			auto const actual_count = upper_limit - offset;
			for(size_t i = 0; i < actual_count; i++)
			{
				data[i] = pcm[offset + i];
			}
			return actual_count;
		};
	}

	template<typename T>
	audio_source<T> resample(audio_source<T> const & src, unsigned int src_sample_rate, unsigned int dst_sample_rate)
	{
		return [=](T * data, std::size_t offset, std::size_t count) -> std::size_t
		{
			auto const wanted_src_count = (count * dst_sample_rate) / src_sample_rate;
			auto const wanted_offset    = (offset * src_sample_rate) / dst_sample_rate;

			xstd::dynamic_array<T> buffer(wanted_src_count);

			auto const actual_src_count = src(buffer.data(), wanted_offset, buffer.size());
			for(size_t dst_i = 0; dst_i < count; dst_i++)
			{
				// TODO: Interpolate samples here
				size_t src_i = (dst_i * src_sample_rate) / dst_sample_rate;
				if(src_i >= actual_src_count)
					return dst_i;
				data[dst_i] = buffer[src_i];
			}
			return count;
		};
	}

	template<typename T>
	std::tuple<std::size_t, std::size_t> unpack_stream_id(mixer<T> const & mixer, stream_id stream)
	{
		auto const chan_index = (stream.value % mixer.channels.size());
		auto const seq_id = (stream.value / mixer.channels.size());
		return std::make_tuple(chan_index, seq_id);
	}

	template<typename T>
	stream_id pack_stream_id(mixer<T> const & mixer, std::size_t channel, std::size_t sequence_id)
	{
		assert(channel <= mixer.channels.size());
		return stream_id { sequence_id * mixer.channels.size() + channel };
	}

	template<typename T>
	stream_id play(mixer<T> & mixer, audio_source<T> const & src, float volume = 1.0f, float pan = 0.0f, float pitch = 1.0f)
	{
		for(size_t i = 0; i < mixer.channels.size(); i++)
		{
			auto & chan = mixer.channels[i];
			if(chan.source)
				continue;
			chan.source = src;
			chan.offset = 0;
			chan.pan = pan;
			chan.pitch = pitch;
			chan.volume = volume;
			chan.sequence_id += 1;
			return pack_stream_id(mixer, i, chan.sequence_id);
		}
		return stream_id::null();
	}

	template<typename T>
	bool is_playing(mixer<T> const & mixer, stream_id stream)
	{
		auto const [ chan_index, seq_id ] = unpack_stream_id(mixer, stream);
		return (mixer.channels[chan_index].sequence_id == seq_id);
	}

	template<typename T>
	xstd::dynamic_array<T> sink(audio_source<T> const & src)
	{
		std::size_t const constexpr block_size = 2048;
		std::size_t count;
		std::size_t offset = 0;
		std::vector<T> buffer;
		do
		{
			auto const startsize = buffer.size();
			buffer.resize(startsize + block_size);
			count = src(buffer.data() + offset, offset, block_size);
			if(count > 0)
				buffer.resize(startsize + count);
			offset += count;
		} while(count == block_size);

		return xstd::dynamic_array<T>(buffer.data(), buffer.size());
	}
}

namespace xsnd::mono
{
	using sample       = mono_sample;
	using mixer        = mixer<sample>;
	using audio_source = audio_source<sample>;
	using pcm_data     = pcm_data<sample>;
}

static void save(char const * fileName, xsnd::mono::audio_source const & src, unsigned int sample_rate)
{
	auto * const f = fopen(fileName, "w");

	std::size_t const constexpr block_size = 2048;
	std::size_t count;
	std::size_t offset = 0;
	std::array<xsnd::mono_sample, block_size> buffer;
	do
	{
		count = src(buffer.data() + offset, offset, block_size);
		for(size_t i = 0; i < count; i++)
		{
			fprintf(f, "%f;%f\n", double(i) / double(sample_rate), double(buffer[i]));
		}
		offset += count;
	} while(count == block_size);

	fclose(f);
}

xstd::dynamic_array<glm::vec3> prepare_lineset(xsnd::mono::pcm_data const & pcm, unsigned int sample_rate)
{
	xstd::dynamic_array<glm::vec3> result(pcm.size());
	for(size_t i = 0; i < result.size(); i++)
	{
		result[i] = glm::vec3(
			float(i) / float(sample_rate),
			pcm[i],
			0.0);
	}
	return result;
}

int main()
{
	xsnd::mono::mixer mixer(16);

	xsnd::audio_engine engine {
		.device = "/dev/snd/pcmC0D3p",
		.freq = 44100,
	};
	engine.set_stream(&mixer); // sets the "render" callback

	std::size_t constexpr const src_sample_rate = 250;
	std::size_t constexpr const dst_sample_rate = 2500;
	std::size_t constexpr const sine_freq   = 10;

	xsnd::mono::pcm_data snd(src_sample_rate);
	for(size_t i = 0; i < snd.size(); i++)
	{
	}

	xsnd::mono::audio_source sine_wave = [=](xsnd::mono::sample * data, std::size_t offset, std::size_t count) -> std::size_t
	{
		for(size_t i = 0; i < count; i++)
		{
			data[i] = std::sin(2.0f * float(M_PI) * ((offset + i) * sine_freq) / src_sample_rate);
		}
		return count;
	};

	auto orig = xsnd::sink(sine_wave);
	auto resampled = xsnd::sink(xsnd::resample(xsnd::make_source(snd), src_sample_rate, dst_sample_rate));

	if(not xapp::init(xapp::opengl))
		return 1;

	auto old_lines = prepare_lineset(orig, src_sample_rate);
	auto new_lines = prepare_lineset(resampled, dst_sample_rate);

	xgraphics::debug_draw debug;
	while(xapp::update())
	{
		using namespace glm;
		debug.clear();

		debug.line(vec3(0, 0, 0), vec3(1, 0, 0));

		auto const paint = [&](xstd::dynamic_array<vec3> const & input, vec4 color)
		{
			for(size_t i = 1; i < input.size(); i++)
			{
				debug.line(input[i - 1], input[i], color);
			}
		};

		paint(old_lines, vec4(1));
		paint(new_lines, vec4(0,1,0,1));

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		debug.draw(ortho(0.0, 1.0, -1.0, 1.0));
		xapp::present();
	}

	xsnd::stream_id id = xsnd::play(mixer, resampled);
	while(xsnd::is_playing(mixer, id))
	{
		// do stuff here
	}
	return 0;
}
*/

/*
int serverfoo()
{
	xnet::http::server server;
	if(not server.bind(xnet::parse_ipv4("0.0.0.0", 8080)))
		return 1;

	std::cout << "ready.\n";

	while(true)
	{
		auto [ request, response ] = server.get_context();
		response.status_code = 200;

		auto stream = response.get_stream();

		stream.write_line("URL:    " + request.url);
		stream.write_line("Method: " + request.method);
		stream.write_line();

		stream.write_line("Headers:");
		for(auto const & header : request.headers)
		{
			stream.write_line(header.first + ": " + header.second);
		}
	}
	return 0;
}
*/
