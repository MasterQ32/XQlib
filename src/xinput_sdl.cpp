#include "../include/xinput/xinput"
#include "../include/xinput/sdl"
#include "../include/xcept"

#include <unordered_map>

namespace
{
	std::unordered_map<SDL_Scancode, xinput::button> buttonmapping;
}

bool xinput::sdl::process_event(SDL_Event const & ev)
{
	if((ev.type == SDL_KEYDOWN) or (ev.type == SDL_KEYUP))
	{
		auto it = buttonmapping.find(ev.key.keysym.scancode);
		if(it == buttonmapping.end())
			return false;
		xinput::set_button(it->second, (ev.type == SDL_KEYDOWN));
		return true;
	}
	return false;
}

void xinput::sdl::clear_mapping()
{
	buttonmapping.clear();
}

void xinput::sdl::clear_mapping_for(button btn)
{
	for(auto it = buttonmapping.begin(); it != buttonmapping.end();)
	{
		if(it->second == btn)
		{
			auto old = it++;
			buttonmapping.erase(old);
		}
		else
		{
			++it;
		}
	}
}

void xinput::sdl::add_mapping(button btn, SDL_Scancode scancode)
{
	if(buttonmapping.emplace(scancode, btn).second == false)
		throw xcept::invalid_argument("scancode already has a button assigned!");
}

void xinput::sdl::remove_mapping(button, SDL_Scancode scancode)
{
	buttonmapping.erase(scancode);
}
