# xq's gamedev library

## XApp

Simple abstraction layer over SDL window and event system,
provides fast application setup.

```cpp
#include <xgl/all>
#include <xapp>

int main()
{
	if(not xapp::init(xapp::opengl))
		return 1;

	while(xapp::update())
	{
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		xapp::present();
	}

	return 0;
}
```

## XGL
thin OpenGL wrapper

### Example:

```cpp
#include <xgl/all>
#include <glm/glm.hpp>

void some()
{
	xgl::program program;
	{
		xgl::shader vsh(GL_VERTEX_SHADER);
		xgl::shader fsh(GL_FRAGMENT_SHADER);

		vsh.source(R"glsl(#version 330
			layout(location = 0) in vec3 vPosition;

			uniform mat4 uTransform;

			void main() {
				gl_Position = uTransform * vec4(vPosition, 1.0);
			}

		)glsl");

		fsh.source(R"glsl(#version 330
			out vec4 color;

			void main() {
				color = vec4(1,0,0,1); // RED!
			}

		)glsl");

		vsh.compile();
		fsh.compile();

		program << vsh << fsh; // attach shaders
		program.link();
		program >> vsh >> fsh; // detach shaders
	}



	// Create vertex buffer with simple vertices
	xgl::buffer vertexBuffer;
	vertexBuffer.data({
		glm::vec3(1.0, 0.0, 0.0),
		glm::vec3(0.0, 1.0, 0.0),
		glm::vec3(0.0, 0.0, 1.0),
	});

	// Create index buffer with signed int data
	xgl::buffer indexBuffer;
	indexBuffer.data({ 0, 1, 2 });

	program.use();
	program.uniform("uTransform", glm::identity<glm::mat4>());
}

```


## XCS
entity component system

```cpp
#include <xcs>

struct SomeComponent
{
	int a, b;
};
XCS_REGISTER_COMPONENT(SomeComponent)

namespace SomeSystem
{
	void update(xcs::universe & universe)
	{
		for(auto [ entity, component ]: xcs::get_components<SomeComponent>(universe))
		{
			std::cout
					<< entity.id
			    << " => ("
			    << component.a
			    << ", "
					<< component.b
			    << ")"
			    << std::endl
			    ;
		}
	}
};

int main()
{
	xcs::universe scene;
	SomeSystem system(scene);

	xcs::entity ent = scene.create_entity();

	auto & c = xcs::add_component<SomeComponent>(ent);
	c.a = 10;
	c.b = 20;

	std::cout << ent.id << ":SomeComponent => " << xcs::get_component<SomeComponent>(ent) << std::endl;

	SomeSystem::update(scene);
	return 0;
}
```

## XM
math library with linear algebra stuff useful for gamedev

## XNet

networking library with multiple levels of networking.

### Socket example

Simple server that accepts exactly one connection and echoes all sent text:

```cpp
int main()
{
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
		std::cout << "client disconnected!" << std::endl;
	}
}
```

### HTTP Server Example

Provides a simple echo service that shows the user the request he made.

```cpp
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

		stream.write_line("URL:    " + request.url);
		stream.write_line("Method: " + request.method);
		stream.write_line();

		stream.write_line("Headers:");
		for(auto const & header : request.headers)
			stream.write_line(header.first + ": " + header.second);
	}
}
```
