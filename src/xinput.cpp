#include "../include/xinput/xinput"
#include "../include/xcept"

#include <vector>


namespace
{
	bool updating = false;
	inline void check_updating()
	{
		if(updating)
			throw xcept::invalid_operation("cannot query input state while updating.");
	}

	struct button_info
	{
		bool is_pressed;
		bool is_hit;
		bool is_released;
	};

	std::vector<button_info> buttonstate;

	button_info & button(xinput::button btn)
	{
		return buttonstate[btn.value];
	}
}

void xinput::begin_update()
{
	if(updating)
		throw xcept::invalid_operation("xinput::begin_update: end_update was not called.");
	updating = true;
	for(auto & info : buttonstate)
	{
		info.is_hit = false;
		info.is_released = false;
	}
}

void xinput::set_button(button btn, bool is_pressed)
{
	if(not updating)
		throw xcept::invalid_operation("xinput::begin_update: begin_update was not called.");
	auto & button = ::button(btn);

	if(button.is_pressed == is_pressed)
		return;

	if(is_pressed)
		button.is_hit = true;
	else
		button.is_released = true;
	button.is_pressed = is_pressed;
}

void xinput::end_update()
{
	if(not updating)
		throw xcept::invalid_operation("xinput::begin_update: begin_update was not called.");
	updating = false;
}

void xinput::register_button(button btn)
{
	buttonstate.resize(std::max(buttonstate.size(), btn.value + 1));
}

bool xinput::pressed(button btn)
{
	check_updating();
	return ::button(btn).is_pressed;
}

bool xinput::hit(button btn)
{
	check_updating();
	return ::button(btn).is_hit;
}

bool xinput::released(button btn)
{
	check_updating();
	return ::button(btn).is_released;
}
