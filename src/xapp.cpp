#include "../include/xapp"
#include "../include/xlog"

#include <GL/gl3w.h>
#include <SDL_opengl.h>

#include <map>

namespace
{
	xapp::mode init_mode = xapp::uninitialized;

	bool quit = false;

	std::map<SDL_EventType, xapp::event_handler> event_handlers;

	xlog::log appError() { return xlog::log("xapp", xlog::error); }
}

SDL_Window * xapp::window = nullptr;
SDL_Renderer * xapp::renderer = nullptr;
SDL_GLContext xapp::glcontext = nullptr;

xapp::options::options(std::string const & title, glm::ivec2 resolution, bool fullscreen) :
  title(title),
  resolution(resolution),
  fullscreen(fullscreen)
{

}

xapp::mode xapp::was_init()
{
	return init_mode;
}

bool xapp::init(xapp::mode mode, options const & opt)
{
	if(mode == xapp::uninitialized)
	{
		appError() << "xapp::init(uninitialized) is not supported!";
		return false;
	}
	if(init_mode != xapp::uninitialized)
	{
		appError() << "xapp was already initialized!";
		return false;
	}
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		appError() << SDL_GetError();
		return false;
	}

	if(mode == xapp::opengl)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG | SDL_GL_CONTEXT_DEBUG_FLAG);
	}

	Uint32 flags = SDL_WINDOW_SHOWN;
	if(mode == xapp::opengl)
		flags |= SDL_WINDOW_OPENGL;
	if(opt.fullscreen)
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

	xapp::window = SDL_CreateWindow(
		opt.title.c_str(),
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		opt.resolution.x, opt.resolution.y,
		flags);
	if(window == nullptr)
	{
		appError() << SDL_GetError();
		return false;
	}

	switch(mode)
	{
		case xapp::uninitialized:
			abort();
		case xapp::opengl:
		{
			xapp::glcontext = SDL_GL_CreateContext(window);
			if(xapp::glcontext == nullptr)
			{
				appError() << SDL_GetError();
				SDL_DestroyWindow(xapp::window);
				xapp::window = nullptr;
				return false;
			}

			if(gl3wInit() < 0)
			{
				appError() << "xapp: Failed to initialize gl3w!";
				SDL_GL_DeleteContext(xapp::glcontext);
				SDL_DestroyWindow(xapp::window);
				xapp::window = nullptr;
				xapp::glcontext = nullptr;
				return false;
			}
			xlog::log("xapp", xlog::message) << "OpenGL Version: " << glGetString(GL_VERSION);
			xlog::log("xapp", xlog::message) << "OpenGL Vendor:  " << glGetString(GL_VENDOR);
			xlog::log("xapp", xlog::message) << "GLSL Version:   " << glGetString(GL_SHADING_LANGUAGE_VERSION);
			break;
		}
		case xapp::sdl:
		{
			abort();
		}
	}

	xapp::set_event_handler<SDL_QUIT>([](auto const &)
	{
		quit = true;
	});

	init_mode = mode;

	return true;
}

bool xapp::update()
{
	SDL_Event ev;
	while(SDL_PollEvent(&ev))
	{
		auto h = event_handlers.find(SDL_EventType(ev.type));
		if(h != event_handlers.end())
			h->second(ev);
		else
			xlog::log("xapp", xlog::verbose) << "Unhandled SDL event " << ev.type;
	}

	return not quit;
}

void xapp::present()
{
	if(init_mode == xapp::uninitialized)
	{
		appError() << "xapp is not initialized!";
		return;
	}
	switch(init_mode)
	{
		case xapp::uninitialized:
		case xapp::opengl:
			SDL_GL_SwapWindow(xapp::window);
			break;
		case xapp::sdl:
			SDL_RenderPresent(xapp::renderer);
			break;
	}
}


void xapp::set_event_handler(SDL_EventType type, event_handler const & handler)
{
	event_handlers[type] = handler;
}

void xapp::remove_event_handler(SDL_EventType type)
{
	event_handlers.erase(type);
}
