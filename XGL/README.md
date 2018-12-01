# XGL
xq's thin OpenGL wrapper

## Example:

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
