#include "../include/xm/3d/aabb"
#include "../include/xm/3d/plane"
#include "../include/xm/3d/ray"
#include "../include/xm/3d/intersect"

xstd::optional<xm3d::ray_box_hit> xm3d::intersect(aabb const & box, ray const & ray)
{
    // compute intersection of ray with all six bbox planes
    glm::vec3 invR = glm::vec3(1.0f, 1.0f, 1.0f) / ray.direction;
    glm::vec3 tbot = invR * (box.min - ray.origin);
    glm::vec3 ttop = invR * (box.max - ray.origin);

    // re-order intersections to find smallest and largest on each axis
    glm::vec3 tmin = min(ttop, tbot);
    glm::vec3 tmax = max(ttop, tbot);

    // find the largest tmin and the smallest tmax
    float largest_tmin = glm::max(glm::max(tmin.x, tmin.y), tmin.z);
    float smallest_tmax = glm::min(glm::min(tmax.x, tmax.y), tmax.z);

    // *tnear = largest_tmin;
    // *tfar = smallest_tmax;

    if(smallest_tmax > largest_tmin)
			return ray_box_hit { largest_tmin, smallest_tmax };
		else
			return xstd::nullopt;
}

xstd::optional<float> xm3d::intersect(xm3d::plane const & plane, xm3d::ray const & ray)
{
	// assuming vectors are all normalized
	float denom = dot(plane.normal(), ray.direction);
	if (abs(denom) <= 1e-6f)
		return xstd::nullopt;
	auto const p0l0 = plane.origin - ray.origin;
	return dot(p0l0, plane.normal()) / denom;
}
