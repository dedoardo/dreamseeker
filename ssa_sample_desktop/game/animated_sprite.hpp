#pragma once

#include <stegosaurus.hpp>

class AnimatedSprite : public ssa::Sprite
{
public :
	AnimatedSprite();

	void set(ssa::float2 p_rect, float p_transition, ssa::Texture& p_texture, bool inverted = false);
	void update(unsigned int p_milliseconds);

	ssa::float2 get_rect()const { return m_rect; }

private :
	ssa::float2 m_rect;
	float		m_transition;
	float		m_time;
	unsigned int m_animation_index;
	std::vector<ssa::float2> m_rects;
};