#include "../include/xutility/debug_camera"
#include <glm/gtc/quaternion.hpp>

using xutility::debug_camera;


void debug_camera::look(float horiz, float vert)
{
	pan  -= horiz;
	tilt = glm::clamp(
		tilt - vert,
		-glm::radians(90.0f),
		 glm::radians(90.0f)
	);
}

glm::quat debug_camera::get_view_rotation() const
{
	return glm::angleAxis(pan, glm::vec3(0,1,0))
			 * glm::angleAxis(tilt, glm::vec3(1,0,0))
			 ;
}

void debug_camera::move(float forward, float sidewards)
{
	auto const rotation = get_view_rotation();

	auto const fwd_dir   = speed * (rotation * glm::vec3(0, 0, -1));
	auto const right_dir = speed * (rotation * glm::vec3(1, 0, 0));

	position += forward   * fwd_dir;
	position += sidewards * right_dir;
}

glm::vec3 debug_camera::get_view_direction() const
{
	return get_view_rotation() * glm::vec3(0, 0, -1);
}

glm::mat4 debug_camera::get_view_matrix() const
{
	auto const rotation = get_view_rotation();
	return glm::lookAt(
		position,
		position + get_view_direction(),
		rotation * glm::vec3(0, 1, 0));
}

glm::mat4 debug_camera::get_projection_matrix(float aspect) const
{
	return glm::perspective<float>(
		fieldOfView,
		aspect,
		clip_near, clip_far
	);
}
