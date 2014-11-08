//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "../ssa_renderer.hpp"
#include "../ssa_buffer.hpp"
#include "../ssa_common_vertex_formats.hpp"
#include "../ssa_shader.hpp"
#include "../ssa_sampler.hpp"

namespace ssa
{
	class RenderDevice;
	class Renderable2D;

	class Renderer2D : public Renderer
	{
		static const std::size_t default_buffer_size{ 1296 };

		// @TODO : implement multiple texture binding
		// static const std::size_t max_bound_textures{ 16 };
	
	public :
		enum class SortMode
		{
			None,
			Texture,
			FrontToBack,
			BackToFront,
		};

	public :
		Renderer2D(RenderDevice& p_render_device, std::size_t p_initial_buffer_size = default_buffer_size);
		~Renderer2D();

		void begin(SortMode p_sort_mode);

		// Not const cause it might trigger renderable triangulation
		void render(Renderable2D& p_renderable);
		void end();

	protected :
		void _flush();
		void _set_states();

	private :
		SortMode	m_sort_mode;

		Buffer		m_graphics_buffer;
		VertexPCT*	m_raw_buffer;
		std::vector<std::reference_wrapper<Renderable2D>> m_renderables;

		std::size_t m_buffer_size;
		std::size_t m_last_element;

		Shader		m_vertex_shader;
		Shader		m_pixel_shader;
		Sampler		m_texture_sampler;
	};
}