#include "fragment_manager.hpp"
#include "../map/map_structs.hpp"
#include "AABB.hpp"
#include <utility/ssa_loaders.hpp>
#include <iostream>

FragmentManager::FragmentManager() : 
m_map{ nullptr }
{
	m_fragment_texture = nullptr;
}

FragmentManager::~FragmentManager()
{
	delete m_fragment_texture;
}

void FragmentManager::init(Map& p_map, ssa::RenderDevice& p_render_device, ssa::Renderer2D& p_renderer2D)
{
	m_map = &p_map;
	m_render_device = &p_render_device;
	m_renderer2D = &p_renderer2D;

	m_bitflag.resize(p_map.fragments.size());
	for (auto& flag : m_bitflag) flag = 1;

	ssa::loaded_data data;
	unsigned long width, height;
	if (!ssa::load_png("assets/fragment.png", data, width, height))
	{
		std::cout << "Failed to load fragment data" << std::endl;
	}

	m_fragment_texture = new ssa::Texture(p_render_device);
	if (!m_fragment_texture->create(width, height, ssa::Format::RGBA8Unorm, false, &data[0]))
	{
		std::cout << "Failed to create star's textures" << std::endl;
	}

	for (unsigned int i = 0; i < p_map.fragments.size(); ++i)
	{
		AnimatedSprite animated_sprite;
		animated_sprite.position = p_map.fragments[i].position;
		animated_sprite.set(ssa::float2(32, 32), 0.05f, *m_fragment_texture);
		m_fragments.push_back(animated_sprite);
	}
}

void FragmentManager::update(ssa::float2 p_player_position, ssa::float2 p_player_size, int p_milliseconds)
{
	AABB player(p_player_position, p_player_size);

	for (unsigned int i = 0; i < m_fragments.size(); ++i)
	{
		if (m_bitflag[i])
		{
			m_fragments[i].update(p_milliseconds);
			AABB fragment(m_fragments[i].position, m_fragments[i].get_rect());
			if (fragment.collides_with(player))
				m_bitflag[i] = false;
		}
	}
}

void FragmentManager::render(ssa::float2 p_displacement)
{
	for (auto& fragment : m_fragments)
		fragment.position += p_displacement;

	m_renderer2D->begin(ssa::Renderer2D::SortMode::None);
	for (unsigned int i = 0; i < m_fragments.size(); ++i)
	{
		if (m_bitflag[i])
			m_renderer2D->render(m_fragments[i]);
	}
	m_renderer2D->end();

	for (auto& fragment : m_fragments)
		fragment.position -= p_displacement;
}