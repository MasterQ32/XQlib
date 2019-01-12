#include "../include/xnet/http/client"
#include "../include/xnet/http/server"
#include "../include/xnet/http/utility"
#include "../include/xnet/socket"
#include "../include/xnet/socket_stream"
#include "../include/xnet/dns"
#include "../include/xnet/ip"
#include "../include/xlog"
#include "../include/xcept"

#include <curl/curl.h>
#include <curl/easy.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>

#include <gsl/gsl>

#include <cstring>
#include <regex>

template<typename Dst, typename Src>
static Dst memblit(Src const & val)
{
	static_assert(sizeof(Dst) >= sizeof(Src));
	static_assert(std::is_trivial_v<Dst>);
	static_assert(std::is_trivial_v<Src>);
	Dst dst;
	memset(&dst, 0, sizeof dst);
	memcpy(&dst, &val, sizeof val);
	return dst;
}

xnet::endpoint::endpoint()
{
	memset(&value, 0, sizeof value);
}

xnet::endpoint::endpoint(sockaddr const & addr) :
  value(memblit<sockaddr_storage>(addr))
{

}

xnet::endpoint::endpoint(sockaddr_in const & addr) :
  value(memblit<sockaddr_storage>(addr))
{

}

xnet::endpoint::endpoint(sockaddr_in6 const & addr) :
  value(memblit<sockaddr_storage>(addr))
{

}

xnet::dns::dns_entry::dns_entry() :
  flags(0),
  family(0),
  socket_type(0),
  protocol(0),
  address(),
  canonname()
{
	memset(&address, 0, sizeof address);
}

// in: xnet/dns
std::vector<xnet::dns::dns_entry> xnet::dns::resolve(std::string const & hostname, std::string const & service, int socket_type, int address_family)
{
	addrinfo hint;
	addrinfo * results;

	memset(&hint, 0, sizeof hint);
	hint.ai_family = address_family;
	hint.ai_socktype = socket_type;
	hint.ai_flags = 0;
	hint.ai_protocol = 0;

	auto const err = getaddrinfo(
			hostname.empty() ? nullptr : hostname.c_str(),
			service.empty()  ? nullptr : service.c_str(),
			&hint,
			&results);
	if(err != 0)
	{
		xlog::log("xnet::dns::resolve", xlog::error) << "Could not resolve " << hostname << ":" << service << ": " << gai_strerror(err);
		return { };
	}

	assert(results != nullptr);

	std::vector<dns_entry> addresses;
	for(auto * it = results; it != nullptr; it = it->ai_next)
	{
		auto & entry = addresses.emplace_back();
		assert(sizeof(entry.address) >= it->ai_addrlen);
		memcpy(&entry.address, it->ai_addr, it->ai_addrlen);

		if(it->ai_canonname != nullptr)
			entry.canonname = std::string(it->ai_canonname);

		entry.family = it->ai_family;
		entry.flags = it->ai_flags;
		entry.protocol = it->ai_protocol;
		entry.socket_type = it->ai_socktype;
	}

	freeaddrinfo(results);

	return addresses;
}

// in: xnet/socket
xnet::endpoint xnet::parse_ipv4(char const * addr_str, int port)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof addr);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(addr_str);
	addr.sin_port = htons(gsl::narrow<uint16_t>(port));

	return { addr };
}

// in: xnet/socket
xnet::endpoint xnet::parse_ipv6(char const * addr_str, int port)
{
	sockaddr_in6 addr;
	memset(&addr, 0, sizeof addr);
	addr.sin6_family = AF_INET6;
	addr.sin6_port = htons(gsl::narrow<uint16_t>(port));
	inet_pton(AF_INET6, addr_str, &addr.sin6_addr);

	return { addr };
}


std::string xnet::to_string(endpoint const & addr)
{
	std::array<char, std::max(INET6_ADDRSTRLEN, INET_ADDRSTRLEN) + 1> text;
	switch(addr.family())
	{
	    case AF_INET:
	        inet_ntop(AF_INET, &reinterpret_cast<sockaddr_in const *>(&addr.value)->sin_addr, text.data(), text.size());
	        break;

	    case AF_INET6:
	        inet_ntop(AF_INET6, &reinterpret_cast<sockaddr_in6 const *>(&addr.value)->sin6_addr, text.data(), text.size());
	        break;

	    default:
				xlog::log("xnet::to_string", xlog::error) << "unsupported address family: " << addr.family();
				return "";
	}
	return std::string(text.data());
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











xnet::socket::socket(int socket_handle) :
  handle(socket_handle)
{

}

xnet::socket::socket(int domain, int type, int proto) :
  handle(::socket(domain, type, proto))
{

}

xnet::socket::socket(socket && other) :
  handle(other.handle)
{
	other.handle = -1;
}

xnet::socket::~socket()
{
	if(handle != -1)
		close(handle);
}

bool xnet::socket::bind(xnet::endpoint const & addr)
{
	return (::bind(handle, addr.data(), addr.size()) >= 0);
}

bool xnet::socket::listen(int backlog)
{
	return (::listen(handle, backlog) >= 0);
}


std::tuple<xnet::socket, xnet::endpoint> xnet::socket::accept()
{
	xnet::endpoint addr;
	socklen_t addr_len = addr.size();

	auto const sock = ::accept(handle, addr.data(), &addr_len);
	if(sock == -1)
		throw xcept::io_error("failed to accept socket!");

	return std::make_tuple(xnet::socket(sock), addr);
}

bool xnet::socket::connect(xnet::endpoint const & addr)
{
	return (::connect(handle, addr.data(), addr.size()) >= 0);
}


ssize_t xnet::socket::read(void * data, size_t length, int flags)
{
	return ::recv(handle, data, length, flags);
}

ssize_t xnet::socket::write(void const * data, size_t length, int flags)
{
	return ::send(handle, data, length, flags);
}

std::tuple<ssize_t, xnet::endpoint> xnet::socket::read_from(void * data, size_t length, int flags)
{
	xnet::endpoint addr;
	socklen_t addr_len = addr.size();
	auto const count = ::recvfrom(handle, data, length, flags, addr.data(), &addr_len);
	return std::make_tuple(count, addr);
}

ssize_t xnet::socket::write_to(endpoint const & target, void const * data, size_t length, int flags)
{
	return ::sendto(handle, data, length, flags, target.data(), target.size());
}


xnet::socket_istream::socket_istream(socket & _sock) :
  sock(_sock)
{

}

xnet::socket_ostream::~socket_ostream()
{

}

xnet::socket_ostream::socket_ostream(socket & _sock) :
  sock(_sock)
{

}

xnet::socket_istream::~socket_istream()
{

}

xnet::socket_stream::socket_stream(socket & _sock) :
  socket_ostream(_sock),
  socket_istream(_sock)
{

}

xnet::socket_stream::~socket_stream()
{

}

size_t xnet::socket_ostream::write_raw(void const * data, size_t length)
{
	auto const count = sock.write(data, length, 0);
	if(count < 0)
		throw xcept::io_error("failed to send data.");
	return gsl::narrow<size_t>(count);
}

size_t xnet::socket_istream::read_raw(void * data, size_t length)
{
	auto const count = sock.read(data, length, 0);
	if(count < 0)
		throw xcept::io_error("failed to send data.");
	return gsl::narrow<size_t>(count);
}








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
