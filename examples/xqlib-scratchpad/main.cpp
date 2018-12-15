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

#include <tuple>

#include <iostream>

int main()
{
	xnet::http::server server;
	server.bind(xnet::parse_ipv4("0.0.0.0", 80));

	server.start();
	while(true)
	{
		auto [ request, response ] = server.get_context();
		if(request.method == "GET")
		{
			response.status_code = 202;
			response.write("you requested the url '");
			response.write(request.url);
			response.write("'. be happy with this display!");
		}
		else
		{
			response.status_code = 405;
			response.write("not supported.");
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
