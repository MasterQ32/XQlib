#include "../include/xnet/socket"
#include "../include/xnet/socket_stream"
#include "../include/xnet/dns"
#include "../include/xnet/ip"
#include "../include/xlog"
#include "../include/xcept"

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

bool xnet::socket::shutdown(int how)
{
	if(::shutdown(handle, how) == -1)
		return false;
	return true;
}


xnet::socket_istream::socket_istream(socket & _sock) :
  sock(&_sock)
{

}

xnet::socket_ostream::~socket_ostream()
{

}

xnet::socket_ostream::socket_ostream(socket & _sock) :
  sock(&_sock)
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
	auto const count = sock->write(data, length, 0);
	if(count < 0)
		throw xcept::io_error(strerror(errno));
	return gsl::narrow<size_t>(count);
}

size_t xnet::socket_istream::read_raw(void * data, size_t length)
{
	auto const count = sock->read(data, length, 0);
	if(count < 0)
		throw xcept::io_error(strerror(errno));
	return gsl::narrow<size_t>(count);
}







