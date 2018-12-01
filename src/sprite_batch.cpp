#include <x2d/sprite_batch>

namespace
{
	char const * vertex_shader =
		R"glsl(#version 330
	    layout(location = 0) in vec2 vPos;
	    layout(location = 1) in vec2 vUV;

	    uniform mat4 uTransform;

			out vec2 uv;
			void main()
			{
				uv = vUV;
				gl_Position = uTransform * vec4(vPos,0,1);
			}
		)glsl";
	char const * fragment_shader =
		R"glsl(#version 330
			in vec2 uv;
			out vec4 fragment;

			uniform sampler2D uTexture;

			void main()
			{
				fragment = texture(uTexture, uv);
			}
		)glsl";
}

x2d::sprite_batch::sprite_batch() :
  program(),
  vao(),
  vertex_buffer(),
  vertices()
{
	xgl::shader vsh(GL_VERTEX_SHADER);
	xgl::shader fsh(GL_FRAGMENT_SHADER);

	vsh.source(vertex_shader);
	fsh.source(fragment_shader);

	vsh.compile();
	fsh.compile();

	program << vsh << fsh;
	program.link();
	program >> vsh >> fsh;

	vao.enableAttribute(0);
	vao.enableAttribute(1);

	vao.attributeFormat(0, 2, GL_FLOAT, offsetof(vertex, pos));
	vao.attributeFormat(1, 2, GL_FLOAT, offsetof(vertex, uv));

	vao.bindAttribute(0, 0);
	vao.bindAttribute(1, 0);

	vao.vertexBuffer(0, vertex_buffer, 0, sizeof(vertex));
}

void x2d::sprite_batch::begin(glm::mat4 const & transform)
{
	this->transform = transform;
	vertices.clear();
	ranges.clear();
}

void x2d::sprite_batch::draw(xgl::texture2D const & tex, glm::vec2 const & pos, glm::vec2 const & size)
{
	glm::vec2 const faces[] =
	{
	  { 0, 0 }, { 0, 1 }, { 1, 0 },
	  { 1, 0 }, { 0, 1 }, { 1, 1 }
	};

	auto const offset = vertices.size();
	for(auto const & uv : faces)
	{
		vertices.push_back({ pos + size * uv, uv });
	}
	if((ranges.size() == 0) or (ranges.back().tex != tex))
	{
		ranges.push_back( { tex, offset, 0 } );
	}
	ranges.back().count += 6;
}

void x2d::sprite_batch::end()
{
	if(vertices.size() == 0)
		return;
	vertex_buffer.data(vertices, GL_DYNAMIC_DRAW);

	program.use();
	program.uniform("uTexture", 0);
	program.uniform("uTransform", transform);

	vao.bind();
	for(auto const & range : ranges)
	{
		range.tex.bind(0);
		glDrawArrays(
			GL_TRIANGLES,
			gsl::narrow<GLsizei>(range.offset),
			gsl::narrow<GLsizei>(range.count));
	}
}
