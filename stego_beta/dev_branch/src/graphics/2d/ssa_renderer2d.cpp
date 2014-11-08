//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <graphics/2d/ssa_renderer2d.hpp>

// ssa
#include <graphics/ssa_renderer.hpp>
#include <graphics/ssa_texture.hpp>
#include <graphics/ssa_render_device.hpp>
#include <graphics/2d/ssa_renderable2d.hpp>
#include <core/ssa_platform.hpp>

// C++ STD
#include <cstdlib>
#include <cassert>
#include <algorithm>

namespace
{
	const std::string vertex_shader_code
	{
		"struct VS_INPUT"
		"{"
		"float3 Position : POSITION0;"
		"float4 Color : COLOR0;"
		"float2 TexCoord : TEXCOORD0;"
		"};"

		"struct VS_OUTPUT"
		"{"
		"float4 Position : SV_POSITION0;"
		"float4 Color : COLOR0;"
		"float3 TexCoord : TEXCOORD0;"
		"};"

		"VS_OUTPUT main(VS_INPUT input)"
		"{"
		"VS_OUTPUT output;"
		"output.Position = float4(input.Position, 1.f);"
		"output.Color = input.Color;"
		"output.TexCoord = float3(input.TexCoord.x, input.TexCoord.y, 0);"
		"return output;"
		"}"
	};

	const std::string vertex_shader_entry_point{ "main" };

	const std::string pixel_shader_code 
	{

		"struct VS_OUTPUT"
		"{"
		"float4 Position : SV_POSITION0;"
		"float4 Color : COLOR0;"
		"float2 TexCoord : TEXCOORD0;"
		"};"

		"cbuffer PerBatch"
		"{"
			"uint is_texture_bound; " // Change to bool , as soon as it's supported
		"};"

		"Texture2D diffuse_texture : register(t0);"
		"SamplerState diffuse_sampler : register(s0);"

		"float4 main(VS_OUTPUT input) : SV_TARGET"
		"{"
		"float4 result_color = input.Color;"
		"if (is_texture_bound != 0)"
		"{"
		"float4 diffuse_color = diffuse_texture.Sample(diffuse_sampler, input.TexCoord);"
		"result_color *= diffuse_color;"
		"}"
		"return result_color;"
		"}"
	};

	const std::string pixel_shader_entry_point = "main";
}

namespace ssa
{
	Renderer2D::Renderer2D(RenderDevice& p_render_device, std::size_t p_initial_buffer_size) :
		Renderer{ p_render_device, *this },
		m_sort_mode{ SortMode::None },
		m_raw_buffer{ nullptr },
		m_buffer_size{ p_initial_buffer_size },
		m_last_element{ 0 },

		m_graphics_buffer{ p_render_device },
		m_vertex_shader{ p_render_device },
		m_pixel_shader{ p_render_device },
		m_texture_sampler{ p_render_device }
	{
		// Allocating memory for the buffer - doing it here for better error reporting ? i guess ? find a new way to do allocation
		m_raw_buffer = new VertexPCT[m_buffer_size];

		if (!m_render_device.create_buffer(BufferType::Vertex, sizeof(VertexPCT), m_buffer_size, true, nullptr, m_graphics_buffer))
			std::abort();

 		if (!m_render_device.create_shader(ShaderType::Vertex, vertex_shader_code, vertex_shader_entry_point, shader_macro_t(), m_vertex_shader))
			std::abort();

		if (!m_render_device.create_shader(ShaderType::Pixel, pixel_shader_code, pixel_shader_entry_point, shader_macro_t(), m_pixel_shader))
			std::abort();

		if (!m_render_device.create_sampler(SamplerFilterType::Anisotropic, m_texture_sampler))
			std::abort();
	}

	Renderer2D::~Renderer2D()
	{

	}

	void Renderer2D::begin(SortMode p_sort_mode)
	{
		if (!Renderer::can_begin())
			return;
		
		m_sort_mode = p_sort_mode;

		_set_states();
	}

	void Renderer2D::render(Renderable2D& p_renderable)
	{
		if (!can_render())
			return;

		assert(m_raw_buffer != nullptr);

		// WTF is this ?
		if (p_renderable.get_triangulated_vertices().size() > m_buffer_size)
			return;

		m_renderables.push_back(p_renderable);
	}

	void Renderer2D::end()
	{
		if (!can_end())
			return;

		_flush();

		// @TODO : restore states ?
	}

	ssa_force_inline void update_renderable(Renderable2D& p_renderable,
		VertexPCT* p_buffer)
	{
		// Get RT size
		float half_width = 1280.f / 2;
		float half_height = 720.f / 2;

		// Building rotation matrix ( equal for all points )
		// @TODO : do not use STD sin & cos, use GLM ones instead, 
		// There is probably a bug with the RENAME_FUNCTION SHIT
		float angle_sin = sin(radians(p_renderable.rotation));
		float angle_cos = cos(radians(p_renderable.rotation));

		float2x2 rotation_matrix{
			float2{ angle_cos, -angle_sin },
			float2{ angle_sin, angle_cos }
		};

		// @TODO : Check out glm for SSE operations ( optimization )
		for (unsigned int i{ 0 }; i < p_renderable.get_triangulated_vertices().size(); ++i)
		{
			// Setting color
			p_buffer->color = p_renderable.get_triangulated_vertices()[i].color;

			// Scale
			p_buffer->position.x = (((p_buffer->position.x - p_renderable.origin.x) * p_renderable.scale.x) + p_renderable.origin.x);
			p_buffer->position.y = (((p_buffer->position.y - p_renderable.origin.y) * p_renderable.scale.y) + p_renderable.origin.y);

			// Rotation
			float2 tmp_pos{ p_buffer->position.x, p_buffer->position.y };
			tmp_pos = tmp_pos * rotation_matrix;
			p_buffer->position.x = tmp_pos.x;
			p_buffer->position.y = tmp_pos.y;

			// Translation
			p_buffer->position.x += p_renderable.position.x - p_renderable.origin.x * p_renderable.scale.x;
			p_buffer->position.y += p_renderable.position.y - p_renderable.origin.y * p_renderable.scale.y;

			// Converting to screen space coordinates
			p_buffer->position.x = static_cast<float>((p_buffer->position.x - half_width)) / half_width;
			p_buffer->position.y = -(static_cast<float>((p_buffer->position.y - half_height)) / half_height);
			++p_buffer;
		}
	}

	void Renderer2D::_flush()
	{
		if (m_renderables.empty())
			return;

		assert(m_raw_buffer != nullptr);

		// Preprocessing renderables 
		// @TODO : include sorting 
		if (m_sort_mode == Renderer2D::SortMode::Texture)
		{
			std::sort(m_renderables.begin(), m_renderables.end(),
				[](const std::reference_wrapper<Renderable2D>& p_first, const std::reference_wrapper<Renderable2D>& p_second)
			{
				// @Check pointer size on 64-bit architecture
				return p_first.get().get_texture()->get_id() < p_second.get().get_texture()->get_id();
			});
		}
		else if (m_sort_mode == Renderer2D::SortMode::BackToFront)
		{

		}
		else if (m_sort_mode == Renderer2D::SortMode::FrontToBack)
		{

		}

	//	m_render_device.push_target(m_offscreen_rt, false);
		//m_render_device.clear(m_offscreen_rt, float4(0.f, 0.f, 0.f, 0.f));

		if (!m_pp_effects.empty())
		{
			m_render_device.push_target(m_offscreen_rt, false);
			m_render_device.clear(m_offscreen_rt, float4(0.f, 0.f, 0.f, 0.f));
		}

		std::size_t processed{ 0 };
		while (processed < m_renderables.size())
		{
			// Processing sprites
			std::size_t vertex_count{ 0 };
			std::size_t start_index = processed;
			for (; processed < m_renderables.size(); ++processed)
			{
				auto& renderable = m_renderables[processed].get();
				const auto& triangulated_vertices = renderable.get_triangulated_vertices();
				std::memcpy(m_raw_buffer + vertex_count, &triangulated_vertices[0], sizeof(VertexPCT)* triangulated_vertices.size());

				// Processing renderable information
				update_renderable(renderable, m_raw_buffer + vertex_count);

				if (vertex_count + renderable.get_triangulated_vertices().size() >= m_buffer_size)
					break;

				vertex_count += renderable.get_triangulated_vertices().size();
			}

			UpdateType update_type;
			update_type = UpdateType::NoOverwrite;
			if (vertex_count == m_buffer_size)
				update_type = UpdateType::Discard;

			// Updating data to buffer
			if (!m_render_device.update_buffer(m_graphics_buffer, update_type, m_raw_buffer, sizeof(VertexPCT)* vertex_count))
				return;

			// Now, we will have to loop again through all the entities and find the first textures that will
			// be set in the atlas, now you might ask yourself how does the shade knows which texture it has to use ?
			// We use the pre-generated system VertexID and match against a list of integers to see in which range it resides
			// We bind multiple textures
			std::size_t buffer_offset{ 0 };
			std::size_t batch_size{ 0 };
			std::size_t next_renderable{ start_index };
			const Texture* next_texture{ nullptr };

			while (next_renderable < processed)
			{
				// We get the texture of the current renderable
				next_texture = m_renderables[next_renderable].get().get_texture();

				// Resetting buffer size ( since this is per-draw call )
				batch_size = 0;

				if (next_texture == nullptr)
				{
					// If polygon is untextured we go on 'til we find untextured polygon
					while (next_renderable < processed &&
						m_renderables[next_renderable].get().get_texture() == nullptr)
					{
						batch_size += m_renderables[next_renderable].get().get_triangulated_vertices().size();
						++next_renderable;
					}
				}
				else
				{
					// If polygon is textured we simply go on 'til we find polygons textured the same way
					while (next_renderable < processed &&
						m_renderables[next_renderable].get().get_texture() != nullptr &&
						m_renderables[next_renderable].get().get_texture()->get_id() == next_texture->get_id())
					{
						batch_size += m_renderables[next_renderable].get().get_triangulated_vertices().size();
						++next_renderable;
					}
				}

				// If there is no texture bound we notify the shader not to use any texture
				if (next_texture == nullptr)
				{
					std::uint32_t no_texture_bound{ 0 };
					m_render_device.set_value(m_pixel_shader, "is_texture_bound", &no_texture_bound, sizeof(std::uint32_t));

					// Also unbinding the texture to be sure ( keeping it bound causes some flickering ( dunno why tho )) 
					m_render_device.unset_texture(m_pixel_shader, "diffuse_texture");
				}
				else
				{
					std::uint32_t texture_bound{ 1 };
					m_render_device.set_value(m_pixel_shader, "is_texture_bound", &texture_bound, sizeof(std::uint32_t));
					m_render_device.set_texture(m_pixel_shader, "diffuse_texture", *next_texture);
				}
				m_render_device.draw(PrimitiveTopologyType::TriangleList, batch_size, buffer_offset);

				buffer_offset += batch_size;
			}
		}

		// Lighting ? before clearing renderables

		m_renderables.clear();

		// Removing the offscreen target
		if (!m_pp_effects.empty())
			m_render_device.pop_targets();

		//// Postprocessing
		Renderer::_post_process(m_offscreen_rt);
	}

	void Renderer2D::_set_states()
	{
		// Binding shaders
		if (!m_render_device.bind_shader(m_vertex_shader))
			return;
		if (!m_render_device.bind_shader(m_pixel_shader))
			return;

		// Binding vertex buffer
		if (!m_render_device.bind_buffer(m_graphics_buffer))
			return;

		// Binding sampler
		if (!m_render_device.set_sampler(m_pixel_shader, "diffuse_sampler", m_texture_sampler))
			return;
	}
}