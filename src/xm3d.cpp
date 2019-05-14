#include "../include/xm/3d/aabb"
#include "../include/xm/3d/plane"
#include "../include/xm/3d/ray"
#include "../include/xm/3d/sphere"
#include "../include/xm/3d/frustum"
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







xm3d::frustum::frustum(const glm::mat4 & matrix)
{
	std::array<glm::vec4, 8> points =
	{
	  glm::vec4 { -1, -1,  0, 1 }, // left, bottom, near
	  glm::vec4 { -1, -1,  1, 1 }, // left, bottom, far
	  glm::vec4 { -1,  1,  0, 1 }, // left, top, near
	  glm::vec4 { -1,  1,  1, 1 }, // left, top, far
	  glm::vec4 {  1, -1,  0, 1 }, // right, bottom, near
	  glm::vec4 {  1, -1,  1, 1 }, // right, bottom, far
	  glm::vec4 {  1,  1,  0, 1 }, // right, top, near
	  glm::vec4 {  1,  1,  1, 1 }, // right, top, far
	};

	auto const invmat = glm::inverse(matrix);
	for(auto & pt : points)
	{
		pt = invmat * pt;
		pt /= pt.w;
		assert(glm::abs(pt.w - 1.0f) <= 0.01f);
	}

	auto const construct_plane = [&](size_t const (&indices)[4], bool flip) -> plane
	{
		auto const p0 = glm::vec3(points[indices[0]]);
		auto const p1 = glm::vec3(points[indices[1]]);
		auto const p2 = glm::vec3(points[indices[2]]);
		auto const p3 = glm::vec3(points[indices[3]]);

		plane p;
		p.origin = 0.25f * (p0 + p1 + p2 + p3);
		p.tangent = normalize(p0 - p.origin);
		p.bitangent = normalize(p1 - p.origin);
		if(glm::abs(dot(p.tangent, p.bitangent)) > 0.8f)
		   p.bitangent = normalize(p2 - p.origin);

		if(flip)
			p.tangent *= -1.0f;

		return p;
	};

	planes[top] = construct_plane({2, 3, 6, 7}, true);
	planes[bottom] = construct_plane({0, 1, 4, 5}, false);
	planes[left] = construct_plane({0, 1, 2, 3}, true);
	planes[right] = construct_plane({4, 5, 6, 7}, false);
	planes[near] = construct_plane({0, 2, 4, 6}, true);
	planes[far] = construct_plane({1, 3, 5, 7}, false);
}

xm3d::frustum::state xm3d::frustum::test(const glm::vec3 & pos) const
{
	for(auto const & plane : planes)
	{
		auto const p = pos - plane.origin;
		if(dot(p, plane.normal()) < 0)
			return outside;
	}
	return fully_inside;
}

xm3d::frustum::state xm3d::frustum::test(const xm3d::sphere & sphere) const
{
	assert(false and "fix this");
	/*
	bool all_inside = true;
	bool all_outside = true;
	for(auto const & plane : planes)
	{
		auto const p = sphere.center - plane.origin;
		if(auto dist = dot(p, plane.normal()); dist < sphere.radius)
			return outside;
			return (dist >= sphere.radius) ? fully_inside : partially_inside;
	}
	return all_inside ? fully_inside : partially_inside;
	*/
	/*
	 *
	using namespace glm;

	var threshold = -radius;

	for(int i = 0; i < 6; i++)
	{
		float dist = frustrum.planes[i].distance(position);
		if(dist < threshold)
			return true;
	}
	return false;
	*/
}

xm3d::frustum::state xm3d::frustum::test(const xm3d::aabb & box) const
{
	state st = fully_inside;
	for(auto const & plane : planes)
	{
		bool all_inside = true;
		bool all_outside = true;
		for(size_t i = 0; i < 8; i++)
		{
			auto const p = box.min
				+ glm::vec3(!!(i & 4), !!(i & 2), !!(i & 1)) * (box.max - box.min)
			  - plane.origin
				;
			if(dot(p, plane.normal()) > 0)
				all_outside = false;
			else
				all_inside = false;
		}
		if(all_outside)
			return outside;
		if(not all_inside)
			st = partially_inside;
	}
	return st;
}

xm3d::frustum::state xm3d::frustum::test(const xm3d::aabb & box, const glm::mat4 & transform) const
{
	glm::vec3 const positions[] =
	{
	  transform * glm::vec4 { box.corner(0), 1 },
	  transform * glm::vec4 { box.corner(1), 1 },
	  transform * glm::vec4 { box.corner(2), 1 },
	  transform * glm::vec4 { box.corner(3), 1 },
	  transform * glm::vec4 { box.corner(4), 1 },
	  transform * glm::vec4 { box.corner(5), 1 },
	  transform * glm::vec4 { box.corner(6), 1 },
	  transform * glm::vec4 { box.corner(7), 1 },
	};
	return test(positions, 8);
}

xm3d::frustum::state xm3d::frustum::test(const glm::vec3 * points, size_t count) const
{
	state st = fully_inside;
	for(auto const & plane : planes)
	{
		bool all_inside = true;
		bool all_outside = true;
		for(size_t i = 0; i < count; i++)
		{
			auto const p = points[i] - plane.origin;
			if(dot(p, plane.normal()) > 0)
				all_outside = false;
			else
				all_inside = false;
		}
		if(all_outside)
			return outside;
		if(not all_inside)
			st = partially_inside;
	}
	return st;
}
