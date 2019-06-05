#include "../include/xapp"
#include "../include/xlog"
#include "../include/imgui"
#include "../include/xinput/xinput"

#include <GL/gl3w.h>
#include <SDL_opengl.h>
#include <gsl/gsl>
#include <map>
#include <list>
#include <chrono>
#include <xcept>

using std::chrono::high_resolution_clock;

namespace
{
	bool xinput_active = false;

	xapp::mode init_mode = xapp::uninitialized;

	xapp::options init_options;

	bool quit = false;

	bool had_present = false;

	int exit_code = 0;

	std::list<xapp::event_filter> event_filters;

	std::map<SDL_EventType, xapp::event_handler> event_handlers;

	high_resolution_clock::time_point startup_frame, last_frame, this_frame;

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

#ifdef DEBUG
		if(severity == GL_DEBUG_SEVERITY_HIGH)
			throw xcept::opengl_error(msg);
#endif
	}
}

SDL_Window * xapp::window = nullptr;
SDL_Renderer * xapp::renderer = nullptr;
SDL_GLContext xapp::glcontext = nullptr;

namespace xapp
{
	void install_error_handler()
	{
		glDebugMessageCallback(dbg_message, nullptr);
	}
}

xapp::options::options(std::string const & _title, glm::ivec2 _resolution, bool _fullscreen, bool _resizable) :
  title(_title),
  resolution(_resolution),
  fullscreen(_fullscreen),
  resizable(_resizable),
  enable_imgui(true),
  enable_vsync(true),
  num_samples()
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

		if(opt.num_samples)
		{
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, gsl::narrow<int>(*opt.num_samples));
		}
	}

	Uint32 flags = SDL_WINDOW_SHOWN;
	if(mode == xapp::opengl)
		flags |= SDL_WINDOW_OPENGL;
	if(opt.fullscreen)
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	if(opt.resizable)
		flags |= SDL_WINDOW_RESIZABLE;

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
			// xapp does not share it's context
			SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);
			xapp::glcontext = SDL_GL_CreateContext(window);
			if(xapp::glcontext == nullptr)
			{
				appError() << SDL_GetError();
				SDL_DestroyWindow(xapp::window);
				xapp::window = nullptr;
				return false;
			}

			if(opt.enable_vsync)
				SDL_GL_SetSwapInterval(1);
			else
				SDL_GL_SetSwapInterval(0);

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

			if(opt.num_samples)
			{
				glEnable(GL_MULTISAMPLE);
			}

			break;
		}
		case xapp::sdl:
		{
			Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
			if(opt.enable_vsync)
				flags |= SDL_RENDERER_PRESENTVSYNC;
			xapp::renderer = SDL_CreateRenderer(
				xapp::window,
				-1,
				flags);
			if(xapp::renderer == nullptr)
			{
				appError() << SDL_GetError();
				SDL_DestroyWindow(xapp::window);
				xapp::window = nullptr;
				return false;
			}
		}
	}

	xapp::set_event_handler<SDL_WINDOWEVENT>([](SDL_WindowEvent const & ev)
	{
		switch(ev.event)
		{
			case SDL_WINDOWEVENT_RESIZED:
				switch(init_mode)
				{
					case xapp::uninitialized:
					case xapp::opengl:
						glViewport(0, 0, ev.data1, ev.data2);
						break;
					case xapp::sdl:
						/* already taken care of by SDL2 */
						break;
				}
				break;
		}
	});

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
	init_options = opt;

	if(init_options.enable_imgui)
	{
		if(init_mode == xapp::opengl)
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
			//ImGui::StyleColorsClassic();

			// Setup Platform/Renderer bindings
			ImGui_ImplSDL2_InitForOpenGL(xapp::window);
			ImGui_ImplOpenGL3_Init();

			xapp::add_event_filter(ImGui_ImplSDL2_ProcessEvent);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame(xapp::window);
			ImGui::NewFrame();
		}
		else
		{
			xlog::log("xapp", xlog::error) << "imgui cannot be used with non-opengl backend. sorry!";
			// internally disable imgui
			init_options.enable_imgui = false;
		}
	}

	this_frame = last_frame = startup_frame= high_resolution_clock::now();

	return true;
}

bool xapp::update()
{
	last_frame = this_frame;
	this_frame = high_resolution_clock::now();

	if(::xinput_active)
		xinput::begin_update();

	if(not had_present and init_options.enable_imgui)
		ImGui::EndFrame();
	had_present = false;

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

	if(::xinput_active)
		xinput::end_update();

	if(init_options.enable_imgui)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(xapp::window);
		ImGui::NewFrame();
	}

	return not ::quit;
}

void xapp::clear(glm::vec4 const & color)
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
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glClearColor(color.r, color.g, color.b, color.a);
			glClear(GL_COLOR_BUFFER_BIT);
			break;
		case xapp::sdl:
			SDL_SetRenderDrawColor(
					renderer,
					gsl::narrow<Uint8>(std::clamp<int>(static_cast<int>(255.0f * color.r + 0.5f), 0, 255)),
					gsl::narrow<Uint8>(std::clamp<int>(static_cast<int>(255.0f * color.g + 0.5f), 0, 255)),
					gsl::narrow<Uint8>(std::clamp<int>(static_cast<int>(255.0f * color.b + 0.5f), 0, 255)),
					gsl::narrow<Uint8>(std::clamp<int>(static_cast<int>(255.0f * color.a + 0.5f), 0, 255)));
			SDL_RenderClear(renderer);
			break;
	}
}

void xapp::present()
{
	if(init_mode == xapp::uninitialized)
	{
		appError() << "xapp is not initialized!";
		return;
	}

	if(init_options.enable_imgui)
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

	had_present = true;
}

void xapp::enable_xinput(bool enabled)
{
	::xinput_active = enabled;
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

void xapp::shutdown(int ec)
{
	if(::exit_code == 0)
		::exit_code = ec;
	::quit = true;
}

int xapp::quit()
{
	if(init_options.enable_imgui)
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	SDL_GL_DeleteContext(xapp::glcontext);
	SDL_DestroyWindow(xapp::window);

	SDL_Quit();

	return ::exit_code;
}

glm::ivec2 xapp::get_screen_resolution()
{
	glm::ivec2 result;
	SDL_GetWindowSize(xapp::window, &result.x, &result.y);
	return result;
}

float xapp::dt()
{
	auto const ms = std::chrono::duration_cast<std::chrono::microseconds>(this_frame - last_frame).count();
	return float(ms * double(std::micro::num) / double(std::micro::den));
}

float xapp::fps(bool smooth)
{
	float const current = (1.0f / std::max(dt(), 0.001f));
	static float smoothed = 0.0f;
	smoothed = glm::mix(current, smoothed, 0.1f); // smooth out a bit
	return smooth ? smoothed : current;
}

float xapp::time()
{
	auto const ms = std::chrono::duration_cast<std::chrono::microseconds>(this_frame - startup_frame).count();
	return float(ms * double(std::micro::num) / double(std::micro::den));
}
