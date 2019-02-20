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

	struct axis_map
	{
		xinput::axis axis;
		std::unordered_map<SDL_Scancode, float> scancodes;
		std::unordered_map<int, float> keycodes;
	};

	std::unordered_map<xinput::button, button_map, xinput::button::hash> _button_map;
	std::unordered_map<xinput::axis,   axis_map,   xinput::axis::hash>   _axis_map;
	xinput::sdl::sdl_input_module _module;

	button_map & mapping(xinput::button btn)
	{
		auto it = _button_map.find(btn);
		if(it != _button_map.end())
		{
			return it->second;
		}
		auto [ n, c ] = _button_map.emplace(btn, button_map { });
		assert(c);
		n->second.button = btn;
		return n->second;
	}

	axis_map & mapping(xinput::axis axis)
	{
		auto it = _axis_map.find(axis);
		if(it != _axis_map.end())
		{
			return it->second;
		}
		auto [ n, c ] = _axis_map.emplace(axis, axis_map { });
		assert(c);
		n->second.axis = axis;
		return n->second;
	}
}

void xinput::sdl::sdl_input_module::begin_update()
{
	int cnt;
	auto kbd = SDL_GetKeyboardState(&cnt);
	assert(cnt >= SDL_NUM_SCANCODES);
	for(auto const & entry : _axis_map)
	{
		auto const & axis = entry.second;
		for(auto sc : axis.scancodes)
		{
			if(kbd[sc.first] != 0)
				xinput::update_axis_relative(axis.axis, sc.second);
		}
		for(auto key : axis.keycodes)
		{
			auto const sc = SDL_GetScancodeFromKey(key.first);
			if(kbd[sc] != 0)
				xinput::update_axis_relative(axis.axis, key.second);
		}
	}
}

xinput::sdl::sdl_input_module * const xinput::sdl::module = &_module;

bool xinput::sdl::process_event(SDL_Event const & ev)
{
	if((ev.type == SDL_KEYDOWN) or (ev.type == SDL_KEYUP))
	{
		bool used = false;
		for(auto const & entry : _button_map)
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
		for(auto const & entry : _button_map)
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
	_button_map.clear();
}

void xinput::sdl::clear_mapping_for(button btn)
{
	_button_map.erase(btn);
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


void xinput::sdl::add_mapping(axis ax, button_type type, int value, float delta)
{
	switch(type)
	{
		case scancode:
				mapping(ax).scancodes.emplace(static_cast<SDL_Scancode>(value), delta);
				break;
		case keycode:
				mapping(ax).keycodes.emplace(value, delta);
				break;
		case mouse_button:
				assert(false and "mouse_button not supported for axis mapping!");
	}
}
