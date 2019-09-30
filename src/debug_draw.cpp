#include "../include/xgraphics/debug_draw"
#include <glm/gtc/matrix_transform.hpp>

namespace
{
	char const debug_vertex_shader[] = R"glsl(#version 330
		layout(location = 0) in vec3 vPosition;
		layout(location = 1) in vec4 vColor;

		uniform mat4 uTransform;

		out vec4 color;

		void main()
		{
			gl_Position = uTransform * vec4(vPosition, 1.0);
			color = vColor;
		}
	)glsl";

	char const debug_fragment_shader[] = R"glsl(#version 330
		in vec4 color;

		uniform vec4 uColor;
		uniform float uOverride;

		out vec4 fColor;

		void main()
		{
			fColor = mix(color, uColor, uOverride);
		}
	)glsl";
}

xgraphics::debug_draw::debug_draw(bool auto_clear) :
    vertex_buffer(),
    vao(),
    program(),
    point_vertices(),
    line_vertices(),
    tris_vertices(),
    autoClear(auto_clear),
    point_size(5.0f)
{
	{
		xgl::shader vsh(GL_VERTEX_SHADER);
		xgl::shader fsh(GL_FRAGMENT_SHADER);

		vsh.source(debug_vertex_shader);
		fsh.source(debug_fragment_shader);

		vsh.compile();
		fsh.compile();

		program << vsh << fsh;
		program.link();
		program >> vsh >> fsh;
	}

	vao.enableAttribute(0);
	vao.enableAttribute(1);
	vao.attributeFormat(0, 3, GL_FLOAT, offsetof(vertex, position));
	vao.attributeFormat(1, 4, GL_FLOAT, offsetof(vertex, color));
	vao.bindAttribute(0, 0);
	vao.bindAttribute(1, 0);
	vao.vertexBuffer(0, vertex_buffer, 0, sizeof(vertex));
}

void xgraphics::debug_draw::point(glm::vec3 const & p, glm::vec4 const & color)
{
	point_vertices.push_back({ p, color });
}


void xgraphics::debug_draw::line(glm::vec3 const & p0, glm::vec3 const & p1, glm::vec4 const & color)
{
	line_vertices.push_back({ p0, color });
	line_vertices.push_back({ p1, color });
}

void xgraphics::debug_draw::line(glm::vec3 const & p0, glm::vec4 const & c0, glm::vec3 const & p1, glm::vec4 const & c1)
{
	line_vertices.push_back({ p0, c0 });
	line_vertices.push_back({ p1, c1 });
}

void xgraphics::debug_draw::circle(const glm::vec3 & center, float radius, const glm::vec4 & color)
{
	auto const point = [&](int a) -> glm::vec3
	{
		return center + glm::vec3(
			radius * glm::sin(glm::radians<float>(a)),
			radius * glm::cos(glm::radians<float>(a)),
			0
		);
	};
	int constexpr segments = 72;
	int constexpr delta_angle = 360 / segments;

	static_assert(segments > 0 and segments < 360);
	static_assert((360 % segments) == 0);

	for(int i = 0; i < 360; i += delta_angle)
	{
		line(point(i), point(i + delta_angle), color);
	}
}

void xgraphics::debug_draw::tris(glm::vec3 const & p0, glm::vec3 const & p1, glm::vec3 const & p2, glm::vec4 const & color)
{
	tris_vertices.push_back({ p0, color });
	tris_vertices.push_back({ p1, color });
	tris_vertices.push_back({ p2, color });
}

void xgraphics::debug_draw::aabb(glm::vec3 const & min, glm::vec3 const & max, glm::vec4 const & color)
{
	box(min, max, glm::identity<glm::mat4>(), color);
}

void xgraphics::debug_draw::box(glm::vec3 const & min, glm::vec3 const & max, glm::mat4 const & transform, glm::vec4 const & color)
{
	/*
	  0----------1
	  |\         |\
	  | \        | \
	  |  \       |  \
	  |   4----------5
	  2---|------3   |
	   \  |       \  |
		  \ |        \ |
		   \|         \|
		    6----------7
	*/

	std::array<glm::vec3, 8> corners;
	for(size_t i = 0; i < 8; i++)
	{
		glm::vec4 const local(
			glm::mix(min.x, max.x, float(!!(i&1))),
			glm::mix(min.y, max.y, float(!!(i&2))),
			glm::mix(min.z, max.z, float(!!(i&4))),
			1.0f);

		auto const global = transform * local;

		corners[i] = glm::vec3(global.x, global.y, global.z) / global.w;
	}

	std::array<std::pair<size_t, size_t>, 12> edges =
	{{
		{ 0, 1 }, { 0, 2 }, { 1, 3 }, { 2, 3 }, // front
		{ 4, 5 }, { 4, 6 }, { 6, 7 }, { 5, 7 }, // back
		{ 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }, // front to back
	}};
	for(auto const & edge : edges)
		line(corners[edge.first], corners[edge.second], color);
}

void xgraphics::debug_draw::draw(glm::mat4 const & transform)
{
	vao.bind();
	program.use();
	program.uniform("uTransform", transform);
	program.uniform("uColor", glm::vec4(1.0));
	program.uniform("uOverride", 0.0f);

	if(point_vertices.size() > 0)
	{
		glPointSize(point_size);
		vertex_buffer.data(point_vertices, GL_DYNAMIC_DRAW);
		glDrawArrays(
			GL_POINTS,
			0,
			gsl::narrow<GLsizei>(point_vertices.size()));
	}
	if(line_vertices.size() > 0)
	{
		vertex_buffer.data(line_vertices, GL_DYNAMIC_DRAW);
		glDrawArrays(
			GL_LINES,
			0,
			gsl::narrow<GLsizei>(line_vertices.size()));
	}
	if(tris_vertices.size() > 0)
	{
		vertex_buffer.data(tris_vertices, GL_DYNAMIC_DRAW);

		bool const cull = glIsEnabled(GL_CULL_FACE);

		glDisable(GL_CULL_FACE);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.15f, 2.0f);

		glDrawArrays(
			GL_TRIANGLES,
			0,
			gsl::narrow<GLsizei>(tris_vertices.size()));

		glDisable(GL_POLYGON_OFFSET_FILL);

		program.uniform("uOverride", 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(
			GL_TRIANGLES,
			0,
			gsl::narrow<GLsizei>(tris_vertices.size()));
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		program.uniform("uOverride", 0.0f);

		(cull ? glEnable : glDisable)(GL_CULL_FACE);
	}

	if(autoClear)
		clear();
}

void xgraphics::debug_draw::clear()
{
	point_vertices.clear();
	line_vertices.clear();
	tris_vertices.clear();
}
