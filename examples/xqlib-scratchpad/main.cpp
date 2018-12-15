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

#include <iostream>

int main()
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

		stream.write("you requested the url '");
		stream.write(request.url);
		stream.write("'. be happy with this display!\n");

		stream.write("method used: ");
		stream.write(request.method);
		stream.write("\n");

		stream.write("Oh, you also sent those headers:\n");
		for(auto const & header : request.headers)
		{
			stream.write(header.first);
			stream.write(": ");
			stream.write(header.second);
			stream.write("\n");
		}
	}
}

/*
int main()
{
	xlog::enable_colors(true);
	xlog::set_verbosity(xlog::message);

	xnet::socket server(AF_INET, SOCK_STREAM);

	if(not server.bind(xnet::parse_ipv4("127.0.0.1", 8080)))
		return 1;

	if(not server.listen())
		return 2;

	while(true)
	{
		auto [ client, addr ] = *server.accept();
		std::cout << "connection from " << xnet::to_string(addr) << "!" << std::endl;
		while(true)
		{
			std::array<char, 128> data;
			auto const len = client.read(data.data(), data.size());
			if(len == 0)
				break;
			if(len < 0)
				return 3;
			client.write(data.data(), len);
		}
	}
}
*/
