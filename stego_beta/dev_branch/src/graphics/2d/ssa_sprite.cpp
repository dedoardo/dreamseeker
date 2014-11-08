//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <graphics/2d/ssa_sprite.hpp>

// ssa
#include <graphics/ssa_texture.hpp>

namespace ssa
{
	namespace
	{
		const VertexPCT default_vertices[] =
		{
			{ float3{ 0.f, 0.f, 0.5f }, float4{ 1.f, 1.f, 1.f, 1.f }, 0.f, 0.f },
			{ float3{ 100.f, 0.f, 0.5f }, float4{ 1.f, 1.f, 1.f, 1.f }, 1.f, 0.f },
			{ float3{ 100.f, 100.f, 0.5f }, float4{ 1.f, 1.f, 1.f, 1.f }, 1.f, 1.f },
			{ float3{ 0.f, 100.f, 0.5f }, float4{ 1.f, 1.f, 1.f, 1.f }, 0.f, 1.f }
		};
	}

	Sprite::Sprite()
	{
		set_vertices(&default_vertices[0], 4);
	}

	void Sprite::set_texture(const Texture& p_texture)
	{
		Renderable2D::set_texture(p_texture);

		// Resetting vertices
		for (auto& vertex : m_vertices)
			vertex.position = float3(0.f, 0.f, 0.5f);

		// Resizing vertices, manually operating on them
		// Upper-Right corner
		m_vertices[1].position[0] = static_cast<float>(m_texture->get_data().width);

		// Lower-Right corner
		m_vertices[2].position[0] = static_cast<float>(m_texture->get_data().width);
		m_vertices[2].position[1] = static_cast<float>(m_texture->get_data().height);

		// Lower-Left corner
		m_vertices[3].position[1] = static_cast<float>(m_texture->get_data().height);

		// Manually calling triangulate 
		// @TODO : Check if setting m_dirty = true has the same effect
		Renderable2D::_triangulate();
	}

	void Sprite::set_texture_rect(unsigned int p_x, unsigned int p_y, unsigned int p_width, unsigned int p_height)
	{
		if (m_texture)
		{
			m_vertices[0].texture[0] = static_cast<float>(p_x) / m_texture->get_data().width;
			m_vertices[0].texture[1] = static_cast<float>(p_y) / m_texture->get_data().height;

			m_vertices[1].texture[0] = static_cast<float>(p_x + p_width) / m_texture->get_data().width;
			m_vertices[1].texture[1] = static_cast<float>(p_y) / m_texture->get_data().height;

			m_vertices[2].texture[0] = static_cast<float>(p_x + p_width) / m_texture->get_data().width;
			m_vertices[2].texture[1] = static_cast<float>(p_y + p_height) / m_texture->get_data().height;

			m_vertices[3].texture[0] = static_cast<float>(p_x) / m_texture->get_data().width;
			m_vertices[3].texture[1] = static_cast<float>(p_y + p_height) / m_texture->get_data().height;
		}
	}
	void Sprite::set_rect(unsigned int p_width, unsigned int p_height)
	{
		// Resetting vertices
		for (auto& vertex : m_vertices)
			vertex.position = float3(0.f, 0.f, 0.5f);

		// Resizing vertices, manually operating on them
		// Upper-Right corner
		m_vertices[1].position[0] = static_cast<float>(p_width);

		// Lower-Right corner
		m_vertices[2].position[0] = static_cast<float>(p_width);
		m_vertices[2].position[1] = static_cast<float>(p_height);

		// Lower-Left corner
		m_vertices[3].position[1] = static_cast<float>(p_height);

		Renderable2D::_triangulate();
	}
}