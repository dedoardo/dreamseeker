#include "animated_sprite.hpp"

AnimatedSprite::AnimatedSprite() : 
m_rect{ 0.f, 0.f },
m_transition{ 0.f },
m_time{ 0 },
m_animation_index{ 0 }
{

}

void AnimatedSprite::set(ssa::float2 p_rect, float p_transition, ssa::Texture& p_texture, bool inverted)
{
	m_rects.clear();
	m_rect = p_rect;
	m_transition = p_transition;
	m_time = 0;

	for (unsigned int i = 0; i < p_texture.get_data().height; i += p_rect.y)
	{
		for (unsigned int j = 0; j < p_texture.get_data().width; j += p_rect.x)
		{
			if (inverted)
				m_rects.push_back(ssa::float2(i, j));
			else
				m_rects.push_back(ssa::float2(j, i));
		}
	}

	set_texture(p_texture);
}

void AnimatedSprite::update(unsigned int p_milliseconds)
{
	m_time += static_cast<float>(p_milliseconds) / 1000.f;
	if (m_time >= m_transition)
	{
		m_time -= m_transition;
		m_animation_index = m_animation_index + 1 >= m_rects.size() ? 0 : m_animation_index + 1;
		set_texture_rect(m_rects[m_animation_index].x, m_rects[m_animation_index].y, m_rect.x, m_rect.y);
		set_rect(m_rect.x, m_rect.y);
	}
}