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

#include <xnet/http>

#include <tuple>

std::tuple<xgl::texture2D, xgl::vertex_array> make_stuff()
{
	xgl::texture2D tex;
	xgl::vertex_array array;
	return std::make_tuple(std::move(tex), std::move(array));
}

int main()
{
	xlog::enable_colors(true);
	xlog::set_verbosity(xlog::message);
	if(not xapp::init(xapp::opengl, { "XQlib - Experimental Scratchpad" }))
		return 1;

	xgl::texture2D texes[] =
	{
	  *xgraphics::load_texture("n1.png"),
	  *xgraphics::load_texture("n2.png"),
	  *xgraphics::load_texture("n3.png"),
	  *xgraphics::load_texture("n4.png"),
	};

	xgraphics::sprite_batch batch;

	auto res = xnet::http().download("https://mq32.de/public/2b.jpg");
	if(not res)
		return 1;

	auto tex = xgraphics::load_texture(res->data(), res->size());
	if(not tex)
		return 1;

	while(xapp::update())
	{
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		batch.begin(glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f));
		batch.draw(*tex, glm::vec2(0, 0), glm::vec2(600, 800));
		batch.end();

		xapp::present();
	}

	return 0;
}
