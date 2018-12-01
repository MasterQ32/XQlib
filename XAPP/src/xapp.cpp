#include "../include/xapp"
#include <iostream>

namespace
{
	xapp::mode init_mode = xapp::uninitialized;
}

SDL_Window * xapp::window = nullptr;
SDL_Renderer * xapp::renderer = nullptr;
SDL_GLContext xapp::glcontext = nullptr;

xapp::mode xapp::was_init()
{
	return init_mode;
}

bool xapp::init(xapp::mode mode)
{
	if(mode == xapp::uninitialized)
	{
		std::cerr << "xapp::init(uninitialized) is not supported!" << std::endl;
		return false;
	}
	if(init_mode != xapp::uninitialized)
	{
		std::cerr << "xapp was already initialized!" << std::endl;
		return false;
	}
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cerr << SDL_GetError() << std::endl;
		return false;
	}

	if(mode == xapp::opengl)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG | SDL_GL_CONTEXT_DEBUG_FLAG);
	}

	xapp::window = SDL_CreateWindow(
		"XV - Sacrificial Story",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		1280, 720,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if(window == nullptr)
	{
		std::cerr << SDL_GetError() << std::endl;
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
				std::cerr << SDL_GetError() << std::endl;
				SDL_DestroyWindow(xapp::window);
				xapp::window = nullptr;
				return false;
			}

			if(gl3wInit() < 0)
			{
				std::cerr << "xapp: Failed to initialize gl3w!" << std::endl;
				SDL_GL_DeleteContext(xapp::glcontext);
				SDL_DestroyWindow(xapp::window);
				xapp::window = nullptr;
				xapp::glcontext = nullptr;
				return false;
			}

			std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
			break;
		}
		case xapp::sdl:
		{
			abort();
		}
	}
	init_mode = mode;
	return true;
}


void xapp::present()
{
	if(init_mode == xapp::uninitialized)
	{
		std::cerr << "xapp is not initialized!" << std::endl;
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
