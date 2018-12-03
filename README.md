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


