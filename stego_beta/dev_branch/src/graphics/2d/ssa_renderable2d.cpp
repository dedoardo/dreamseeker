//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <graphics/2d/ssa_renderable2d.hpp>

// C++ STD
#include <list>

namespace ssa
{
	Renderable2D::Renderable2D() :
		m_dirty{ true },
		m_texture{ nullptr }
	{

	}

	void Renderable2D::set_color(const float4& p_color)
	{
		// Setting new color on non-triangulated vertices
		for (auto& vertex : m_vertices)
			vertex.color = p_color;

		// Setting new color on triangulated vertices
		for (auto& triangulated_vertex : m_triangulated_vertices)
			triangulated_vertex.color = p_color;
	}

	void Renderable2D::set_texture(const Texture& p_texture)
	{
		m_texture = &p_texture;
	}

	void Renderable2D::set_vertices(VertexPCT const* p_vertices, std::size_t p_count)
	{
		m_vertices.clear();

		// @TODO : write better code with .insert()
		m_vertices.resize(p_count);
		for (unsigned int i{ 0 }; i < p_count; ++i)
			m_vertices[i] = p_vertices[i];

		// Setting dirty flag to true for later triangulation
		m_dirty = true;
	}

	void Renderable2D::set_vertices(const std::vector<VertexPCT>& p_vertices)
	{
		if (!p_vertices.empty())
			set_vertices(&p_vertices[0], p_vertices.size());
	}

	const std::vector<VertexPCT>& Renderable2D::get_triangulated_vertices()
	{
		if (m_dirty)
		{
			_triangulate();
			m_dirty = false;
		}

		return m_triangulated_vertices;
	}

	void Renderable2D::_triangulate()
	{
		m_triangulated_vertices.clear();

		//// @TODO : Implement an index-based algorithm ( should be faster )
		//// Ok, first conclusion, that can be dimostrated by induction is that an 
		//// N-edged polygon has N-2 triangles
		//// N-edged polygon requires N-3 cuts to create N-3 triangles
		//for (unsigned int cut{ 0 }; cut < vertices.size() - 3; ++cut)
		//{

		//}

		// Simple textured quad case, this is just to avoid some overhead
		if (m_vertices.size() == 4)
		{
			m_triangulated_vertices.push_back(m_vertices[3]);
			m_triangulated_vertices.push_back(m_vertices[0]);
			m_triangulated_vertices.push_back(m_vertices[2]);
			m_triangulated_vertices.push_back(m_vertices[2]);
			m_triangulated_vertices.push_back(m_vertices[0]);
			m_triangulated_vertices.push_back(m_vertices[1]);
		}
		else
		{
			// Triangulate more complex polygon
			// Make sure they are ordered in a clockwise way
			std::list<std::reference_wrapper<const VertexPCT>> remaining_vertices;
			for (const auto& vertex : m_vertices)
				remaining_vertices.push_back(vertex);

			// O(n^2)
			// @TODO : Check to see if it works for polygons that require multiple iterations
			// @TODO : READ UP DECLTYPE / DECLVAL and build 
			std::list<std::reference_wrapper<const VertexPCT>>::iterator vertex;
			while (remaining_vertices.size() > 3)
			{
				vertex = remaining_vertices.begin();
				std::size_t vertices_processed{ 0 };
				std::size_t vertices_to_process{ remaining_vertices.size() };
			
				while (vertices_processed <= vertices_to_process && 
					remaining_vertices.size() > 3 && 
					vertex != remaining_vertices.end())
				{
					// Adding a triangle
					// @TODO: modify this ++ -- i had errors when incrementing + 1
					m_triangulated_vertices.push_back(*vertex);
					m_triangulated_vertices.push_back(*(++vertex));
					m_triangulated_vertices.push_back(*(++vertex));
					--vertex;
					--vertex;

					// Clipping ear out
					vertex = remaining_vertices.erase(++vertex);

					vertices_processed += 3;
				}
			}

			assert(remaining_vertices.size() == 3);

			vertex = remaining_vertices.begin();
			m_triangulated_vertices.push_back(*(vertex++));
			m_triangulated_vertices.push_back(*(vertex++));
			m_triangulated_vertices.push_back(*(vertex++));
		}
	}
}