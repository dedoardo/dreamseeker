#pragma once

#include "../map/map_loader.hpp"
#include <vector>
#include "animated_sprite.hpp"

class FragmentManager
{
public :
	FragmentManager();
	~FragmentManager();

	void init(Map& p_map, ssa::RenderDevice& p_render_device, ssa::Renderer2D& p_renderer2D);

	void update(ssa::float2 p_player_position, ssa::float2 p_player_size, int p_milliseconds);
	void render(ssa::float2 p_displacement);

	bool is_over()const
	{
		for (const auto& flag : m_bitflag)
		{
			if (flag)
				return false;
		}
		return true;
	}
private :
	Map* m_map;
	ssa::RenderDevice* m_render_device;
	ssa::Renderer2D* m_renderer2D;
	std::vector<bool> m_bitflag;
	std::vector<AnimatedSprite> m_fragments;
	ssa::Texture* m_fragment_texture;
};