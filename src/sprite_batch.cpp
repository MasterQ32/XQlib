#include <xgraphics/sprite_batch>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

xgraphics::sprite_batch::sprite_batch() :
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

void xgraphics::sprite_batch::begin(glm::mat4 const & _transform)
{
	this->transform = _transform;
	vertices.clear();
	ranges.clear();
}

void xgraphics::sprite_batch::draw(
	xgl::texture2D const & tex,
	glm::vec2 const & position,
	glm::vec2 const & size,
	float rotation,
	glm::vec2 const & center,
	glm::vec2 const & uv_low,
	glm::vec2 const & uv_high)
{
	glm::vec2 const faces[] =
	{
	  { 0, 0 }, { 0, 1 }, { 1, 0 },
	  { 1, 0 }, { 0, 1 }, { 1, 1 }
	};

	auto const offset = vertices.size();
	for(auto const & xy : faces)
	{
		using namespace glm;
		vec3 pos(xy * size, 0);

		pos -= vec3(center, 0);
		pos = angleAxis(rotation, vec3(0, 0, 1)) * pos;
		pos += vec3(center, 0);

		vertex vtx;
		vtx.pos = position + vec2(pos) - center;
		vtx.uv = uv_low + xy * (uv_high - uv_low);

		vertices.push_back(vtx);
	}
	if((ranges.size() == 0) or (ranges.back().tex != tex))
	{
		ranges.push_back( { tex, offset, 0 } );
	}
	ranges.back().count += 6;
}

void xgraphics::sprite_batch::end()
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
