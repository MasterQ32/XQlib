#include "../include/xapp"
#include "../include/xlog"

#include <GL/gl3w.h>
#include <SDL_opengl.h>
#include <gsl/gsl>
#include <map>
#include <list>

namespace
{
	xapp::mode init_mode = xapp::uninitialized;

	bool quit = false;

	std::list<xapp::event_filter> event_filters;

	std::map<SDL_EventType, xapp::event_handler> event_handlers;

	xlog::log appError()
	{
		return xlog::log("xapp", xlog::error);
	}

	// trim from start (in place)
	static inline void ltrim(std::string &s) {
	    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
	        return !std::isspace(ch);
	    }));
	}

	// trim from end (in place)
	static inline void rtrim(std::string &s) {
	    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
	        return !std::isspace(ch);
	    }).base(), s.end());
	}

	// trim from both ends (in place)
	static inline void trim(std::string &s) {
	    ltrim(s);
	    rtrim(s);
	}

	void APIENTRY dbg_message(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam)
	{
		char const * src_name = "gl";
		switch(source)
		{
			case GL_DEBUG_SOURCE_API:             src_name = "gl(api)"; break;
			case GL_DEBUG_SOURCE_APPLICATION:     src_name = "gl(app)"; break;
			case GL_DEBUG_SOURCE_OTHER:           src_name = "gl(other)"; break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: src_name = "gl(shader compiler)"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:     src_name = "gl(third party)"; break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   src_name = "gl(window system)"; break;
		}

		xlog::log_level log_level = xlog::critical;
		switch(severity)
		{
			case GL_DEBUG_SEVERITY_HIGH: log_level = xlog::critical; break;
			case GL_DEBUG_SEVERITY_MEDIUM: log_level = xlog::error; break;
			case GL_DEBUG_SEVERITY_LOW: log_level = xlog::warning; break;
			case GL_DEBUG_SEVERITY_NOTIFICATION: log_level = xlog::message; break;
		}

		(void)type;
		(void)id;
		(void)userParam;

		std::string msg(message, gsl::narrow<std::size_t>(length));
		trim(msg);

		xlog::log(src_name, log_level) << msg;

		if(severity == GL_DEBUG_SEVERITY_HIGH)
			abort();
	}
}

SDL_Window * xapp::window = nullptr;
SDL_Renderer * xapp::renderer = nullptr;
SDL_GLContext xapp::glcontext = nullptr;

xapp::options::options(std::string const & _title, glm::ivec2 _resolution, bool _fullscreen) :
  title(_title),
  resolution(_resolution),
  fullscreen(_fullscreen)
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
	if(not SDL_WasInit(SDL_INIT_EVERYTHING))
	{
		if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
		{
			appError() << SDL_GetError();
			return false;
		}
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

			glDebugMessageCallback(dbg_message, nullptr);

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
		xapp::shutdown();
	});

	xapp::set_event_handler<SDL_KEYDOWN>([](auto const & ev)
	{
		if(ev.keysym.sym == SDLK_ESCAPE)
			xapp::shutdown();
	});

	init_mode = mode;

	return true;
}

bool xapp::update()
{
	SDL_Event ev;
	while(SDL_PollEvent(&ev))
	{
		bool was_filtered = false;
		for(auto const & ef : event_filters)
		{
			if(not ef(ev))
				continue;
			was_filtered = true;
			break;
		}
		if(was_filtered)
			continue;

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

void xapp::add_event_filter(event_filter const & filt)
{
	event_filters.emplace_back(filt);
}

void xapp::set_event_handler(SDL_EventType type, event_handler const & handler)
{
	event_handlers[type] = handler;
}

void xapp::remove_event_handler(SDL_EventType type)
{
	event_handlers.erase(type);
}

void xapp::shutdown()
{
	quit = true;
}
