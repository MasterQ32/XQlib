#include "../include/xinput/xinput"
#include "../include/xcept"

#include <vector>
#include <glm/glm.hpp>


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

	struct axis_info
	{
		xinput::axis_type type;
		float last_value;
		float value;

		bool any_input;
	};

	std::vector<button_info> buttons;
	std::vector<axis_info> axes;

	button_info & V(xinput::button btn)
	{
		return buttons[btn.value];
	}

	axis_info & V(xinput::axis axis)
	{
		return axes[axis.value];
	}
}

void xinput::begin_update()
{
	if(updating)
		throw xcept::invalid_operation("xinput::begin_update: end_update was not called.");
	updating = true;
	for(auto & info : buttons)
	{
		info.is_hit = false;
		info.is_released = false;
	}
	for(auto & axis : axes)
	{
		axis.last_value = axis.value;
		axis.any_input = false;
	}
}

void xinput::update_button(button btn, bool is_pressed)
{
	if(not updating)
		throw xcept::invalid_operation("xinput::begin_update: begin_update was not called.");
	auto & button = V(btn);

	if(button.is_pressed == is_pressed)
		return;

	if(is_pressed)
		button.is_hit = true;
	else
		button.is_released = true;
	button.is_pressed = is_pressed;
}

void xinput::update_axis_absolute(axis _axis, float value)
{
	if(not updating)
		throw xcept::invalid_operation("xinput::begin_update: begin_update was not called.");
	auto & axis = V(_axis);

	switch(axis.type)
	{
		case relative:
			update_axis_relative(_axis, value);
			break;

		case absolute:
			axis.value = value;
			axis.any_input = true;
			break;
	}
}

void xinput::update_axis_relative(axis _axis, float delta)
{
	if(not updating)
		throw xcept::invalid_operation("xinput::begin_update: begin_update was not called.");
	auto & axis = V(_axis);

	switch(axis.type)
	{
		case relative:
			if(not axis.any_input)
				axis.value = 0;
			axis.value += delta;
			axis.any_input = true;
			break;

		case absolute:
			axis.value += delta;
			axis.any_input = true;
			break;
	}
}

void xinput::end_update()
{
	if(not updating)
		throw xcept::invalid_operation("xinput::begin_update: begin_update was not called.");
	updating = false;
	for(auto & axis : axes)
	{
		if(axis.type == relative)
			axis.value = glm::clamp(axis.value, -1.0f, 1.0f);
	}
}

void xinput::register_button(button btn)
{
	buttons.resize(std::max(buttons.size(), btn.value + 1));
}

bool xinput::pressed(button btn)
{
	check_updating();
	return V(btn).is_pressed;
}

bool xinput::hit(button btn)
{
	check_updating();
	return V(btn).is_hit;
}

bool xinput::released(button btn)
{
	check_updating();
	return V(btn).is_released;
}





void xinput::register_axis(axis axis, axis_type type)
{
	axes.resize(std::max(axes.size(), axis.value + 1));
	V(axis).type = type;
}

float xinput::value(axis axis)
{
	check_updating();
	return V(axis).value;
}

float xinput::delta(axis axis)
{
	check_updating();
	if(V(axis).type == relative)
		return 0.0;
	return V(axis).value - V(axis).last_value;
}
