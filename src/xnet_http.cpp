#include "../include/xnet/http/client"
#include "../include/xnet/http/server"
#include "../include/xnet/http/utility"

#include <curl/curl.h>
#include <curl/easy.h>

#include <gsl/gsl>
#include <cstring>
#include <regex>

xnet::http::server::server() :
  socket(AF_INET, SOCK_STREAM, 0)
{

}

bool xnet::http::server::bind(endpoint const & ep)
{
	if(not socket.bind(ep))
		return false;
	if(not socket.listen())
		return false;
	return true;
}

#include <iostream>

namespace
{
	std::regex const http_init_pattern(R"rgx(^([A-Z]+)[ ]+([^ ]*)[ ]+HTTP\/(\d)\.(\d)$)rgx", std::regex::ECMAScript);

	std::regex const http_header_pattern(R"rgx(^([A-Za-z0-9\_\-]+)\:[ ]+(.*)$)rgx", std::regex::ECMAScript);
}

std::tuple<xnet::http::http_request, xnet::http::http_response> xnet::http::server::get_context()
{
	while(true)
	{
	_restart:
		auto [ client, remote ] = socket.accept();
		auto sock = std::make_shared<xnet::socket>(std::move(client));

		http_request request { sock };
		http_response response { sock };

		socket_istream stream { *sock };

		enum { INIT, HEADER } state = INIT;
		for(auto line = stream.read_line(); not line.empty(); line = stream.read_line())
		{
			switch(state)
			{
				case INIT:
				{
					std::smatch match;
					if(not std::regex_match(line, match, http_init_pattern))
						goto _restart;
					request.method = match[1].str(); // method
					request.url = match[2].str();    // url

					auto const version_major = std::strtol(match[3].str().c_str(), nullptr, 10);
					auto const version_minor = std::strtol(match[4].str().c_str(), nullptr, 10);

					if(version_major != 1)
						goto _restart;
					if(version_minor < 0 or version_minor > 1)
						goto _restart;

					state = HEADER;
					break;
				}

				case HEADER:
				{
					std::smatch match;
					if(not std::regex_match(line, match, http_header_pattern))
						goto _restart;
					request.headers.emplace(match[1].str(), match[2].str());
					break;
				}
			}
		}

		return std::make_tuple(std::move(request), std::move(response));
	}
}



xnet::http::http_request::http_request(std::shared_ptr<xnet::socket> sock) :
  socket(sock),
  method("UNKNOWN")
{

}

xnet::socket_istream xnet::http::http_request::get_stream()
{
	return socket_istream { *socket };
}


xnet::http::http_response::http_response(std::shared_ptr<xnet::socket> sock) :
  socket(sock),
  status_code(200),
  headers()
{

}

xnet::socket_ostream xnet::http::http_response::get_stream()
{
	socket_ostream stream { *socket };
	stream.write("HTTP/1.1 ");
	stream.write(std::to_string(this->status_code));
	stream.write(" \n");
	for(auto const & header : headers)
	{
		stream.write(header.first);
		stream.write(": ");
		stream.write(header.second);
		stream.write("\n");
	}
	stream.write("\n");

	return stream;
}



std::string xnet::http::url_encode(std::string const & src)
{
	static char const * const allowed_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.!*()";
	std::string result = "";
	result.reserve(2 * src.size());
	for(size_t i = 0; i < src.size(); i++)
	{
		char c = src[i];
		if(strchr(allowed_chars, c) != nullptr)
			result.append(1, c);
		else {
			static char const * const hexlut = "0123456789ABCDEF";
			result.append("%");
			result.append(1, hexlut[(c & 0xF0) >> 4]);
			result.append(1, hexlut[(c & 0x0F) >> 0]);
		}
	}
	return result;
}

std::string xnet::http::url_decode(std::string const & src)
{
	abort();
}





xnet::http::client::client() noexcept :
	curl(curl_easy_init())
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1); //Prevent "longjmp causes uninitialized stack frame" bug
	curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
#pragma clang diagnostic pop
}

xnet::http::client::client(client && other) :
  curl(other.curl)
{
	other.curl = nullptr;
}

xnet::http::client::~client() noexcept
{
	if(curl != nullptr)
		curl_easy_cleanup(curl);
}

std::optional<std::vector<std::byte>> xnet::http::client::download(std::string const & url)
{
	return download(url, [](float){});
}

std::optional<std::vector<std::byte>> xnet::http::client::download(std::string const & url, std::function<void(float)> const & on_progress)
{
	buffer.clear();
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
#pragma clang diagnostic pop

	xlog::log("http", xlog::verbose) << "download '" << url << "'";

	if(on_progress) on_progress(0.0f);

	CURLcode res = curl_easy_perform(curl);

	if (res != CURLE_OK)
	{
		xlog::log("http", xlog::error) << "download failed: " << curl_easy_strerror(res);
		return std::nullopt;
	}

	if(on_progress) on_progress(100.0f);

	return buffer;
}

std::size_t xnet::http::client::write_data(void * data, size_t size, size_t nmemb, void *stream)
{
	auto & http = *reinterpret_cast<client*>(stream);

	auto const start = http.buffer.size();
	auto const length = size * nmemb;
	http.buffer.resize(http.buffer.size() + length);
	memcpy(&http.buffer[start], data, length);
	return length;
}
