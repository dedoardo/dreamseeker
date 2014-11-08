#pragma once

// Stegosaurus
#include <stegosaurus.hpp>

using namespace ssa;

class AABB
{
public:
	AABB(float2& p_position, float2 p_size)
		: m_position{ p_position }
		, m_size{ p_size }
	{}

	float2 get_position(){ return m_position; }
	float2 get_size() { return m_size; }

	bool collides_with(AABB& p_other);
	bool collides_bottom(AABB& p_other);
	bool collides_top(AABB& p_other);
	bool collides_right(AABB& p_other);
	bool collides_left(AABB& p_other);

private:

	float2 m_position;
	float2 m_size;
};