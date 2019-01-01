#include "../include/xinput/xinput"
#include "../include/xinput/sdl"
#include "../include/xcept"

#include <unordered_map>
#include <unordered_set>
#include <cassert>

namespace
{
	struct button_map
	{
		xinput::button button;
		std::unordered_set<SDL_Scancode> scancodes;
		std::unordered_set<int> keycodes;
		std::unordered_set<int> mousebuttons;
	};

	std::unordered_map<xinput::button, button_map, xinput::button::hash> _map;


	button_map & mapping(xinput::button btn)
	{
		auto it = _map.find(btn);
		if(it != _map.end())
		{
			return it->second;
		}
		auto [ n, c ] = _map.emplace(btn, button_map { });
		assert(c);
		n->second.button = btn;
		return n->second;
	}
}

bool xinput::sdl::process_event(SDL_Event const & ev)
{
	if((ev.type == SDL_KEYDOWN) or (ev.type == SDL_KEYUP))
	{
		bool used = false;
		for(auto const & entry : _map)
		{
			if(entry.second.scancodes.count(ev.key.keysym.scancode) > 0)
			{
				xinput::update_button(entry.first, (ev.type == SDL_KEYDOWN));
				used = true;
			}

			if(entry.second.keycodes.count(ev.key.keysym.sym) > 0)
			{
				xinput::update_button(entry.first, (ev.type == SDL_KEYDOWN));
				used = true;
			}
		}
		return used;
	}
	if((ev.type == SDL_MOUSEBUTTONDOWN) or (ev.type == SDL_MOUSEBUTTONUP))
	{
		bool used = false;
		for(auto const & entry : _map)
		{
			if(entry.second.mousebuttons.count(ev.button.button) > 0)
			{
				xinput::update_button(entry.first, (ev.type == SDL_MOUSEBUTTONDOWN));
				used = true;
			}
		}
		return used;
	}
	return false;
}

void xinput::sdl::clear_mapping()
{
	_map.clear();
}

void xinput::sdl::clear_mapping_for(button btn)
{
	_map.erase(btn);
}

void xinput::sdl::add_mapping(button btn, button_type type, int value)
{
	switch(type)
	{
		case scancode:
				mapping(btn).scancodes.emplace(static_cast<SDL_Scancode>(value));
				break;
		case keycode:
				mapping(btn).keycodes.emplace(value);
				break;
		case mouse_button:
				mapping(btn).mousebuttons.emplace(value);
				break;
	}
}

void xinput::sdl::remove_mapping(button btn, button_type type, int value)
{
	switch(type)
	{
		case scancode:
				mapping(btn).scancodes.erase(static_cast<SDL_Scancode>(value));
				break;
		case keycode:
				mapping(btn).keycodes.erase(value);
				break;
		case mouse_button:
				mapping(btn).mousebuttons.erase(value);
				break;
	}
}
