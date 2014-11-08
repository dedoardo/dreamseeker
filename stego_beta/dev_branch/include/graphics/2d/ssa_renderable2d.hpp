//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "../../core/ssa_math.hpp"
#include "../ssa_common_vertex_formats.hpp"
#include "ssa_transformable2d.hpp"

// C++ STD
#include <vector>

namespace ssa
{
	class Texture;

	//! \brief 2D Convex polygon that supports texturing + vertex colors
	//!
	//!	This class class Represents a 2D convex polygon, concave polygons are not supported yet since the
	//! triangulation algorithm won't work. If you really need a concave polygon, just create it from multiple
	//! convex ones
	class Renderable2D : public Transformable2D
	{
	public :
		//! \brief Creates a new instance of the renderable
		//! 
		//! Sets the texture to null and color defaults to one
		Renderable2D();

		//! \brief Sets a color on all the vertices, if you need specific color per-vertex
		//!		simply pass it specify it when pushing vertices. 
		//! 
		//! This does not trigger triangulation
		//! \param [in] p_color Color that will be set on all vertices
		void set_color(const float4& p_color);
		
		//! \brief Retrieves the color of the specified vertex
		//! 
		//! \param [in] p_vertex Index of the vertex the color will be sampled from
		//! \return Color of the specified vertex
		const float4& get_color(unsigned int p_vertex)const { return m_vertices[p_vertex].color; }

		//! \brief Sets a new texture, that will be used when rendering
		//! \param [in] p_texture New texture that will be used when rendering
		virtual void set_texture(const Texture& p_texture);

		//! \brief Retrieves the currently bound texture
		//! \return Pointer to texture ( no reference since *nullptr ain't good ) 
		Texture const* get_texture()const { return m_texture; }

		//! \brief Sets that vertices of the renderable
		//!
		//! Notes : vertices should be ordered in a clockwise-way ( counterclockwise not tested yet [ might work ] )
		//! @TODO : Check
		//! Vertices are not triangulated immediatly, only when they are requeste
		//! \param [in] p_vertices Array of vertices
		//! \param [in] p_count Number of vertices
		virtual void set_vertices(VertexPCT const* p_vertices, std::size_t p_count);

		//! \brief Interface created for convenience, same as the other set_vertices()
		//! \param [in] p_vertices STD vector of vertices
		virtual void set_vertices(const std::vector<VertexPCT>& p_vertices);

		//! \brief Returns the non-triangulated vertices as specified by the user
		//! \return Constant reference to the non-triangulated vertices
		const std::vector<VertexPCT>& get_vertices()const { return m_vertices; }

		//! \brief Returns the triangulated vertices. If not triangulated before, this is where triangulation happens
		//! \return Constant reference to the internal vector of triangulated vertices
		const std::vector<VertexPCT>& get_triangulated_vertices();

	protected :
		bool					m_dirty;
		std::vector<VertexPCT>  m_vertices;
		std::vector<VertexPCT>	m_triangulated_vertices;

		Texture const*			m_texture;

		void _triangulate();
	};
}