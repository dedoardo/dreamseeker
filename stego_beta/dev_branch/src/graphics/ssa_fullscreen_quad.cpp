//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <graphics/ssa_fullscreen_quad.hpp>

// ssa
#include <graphics/ssa_render_device.hpp>
#include <graphics/ssa_common_vertex_formats.hpp>

namespace ssa
{
	namespace
	{
		const std::string fullscreen_quad_vs 
		{
			"struct VS_INPUT"
			"{"
			"	float3 Position : POSITION0;"
			"	float2 TexCoord : TEXCOORD0;"
			"};"
			
			"struct VS_OUTPUT"
			"{"
			"	float4 Position : SV_POSITION0;"
			"	float2 TexCoord : TEXCOORD0;"
			"};"

			"VS_OUTPUT main(VS_INPUT input)"
			"{"
			"	VS_OUTPUT output;"
			"	input.Position.x -= 2 * (1.0 / float(1920));"
			"	input.Position.y += 2 * (1.0 / float(1080));"
			"	output.Position = float4(input.Position.xyz, 1.f);"
			"	output.TexCoord = input.TexCoord;"
			"	return output;"
			"}"
		};
	}

	FullscreenQuad::FullscreenQuad(RenderDevice& p_render_device) :
		m_render_device{ p_render_device },
		m_buffer{ p_render_device },
		m_vertex_shader{ p_render_device }
	{
		static VertexPT default_vertices[] 
		{
			{ float3{ -1.f, -1.f, 0.5f }, float2{ 0.f, 1.f }},
			{ float3{ -1.f, 1.f, 0.5f }, float2{ 0.f, 0.f } },
			{ float3{ 1.f, -1.f, 0.5f }, float2{ 1.f, 1.f } },
			{ float3{ 1.f, -1.f, 0.5f }, float2{ 1.f, 1.f } },
			{ float3{ -1.f, 1.f, 0.5f }, float2{ 0.f, 0.f } },
			{ float3{ 1.f, 1.f, 0.5f }, float2{ 1.f, 0.f } },

		};

		if (!m_render_device.create_buffer(BufferType::Vertex, sizeof(VertexPT), 6, false, &default_vertices[0], m_buffer))
			std::abort();

		if (!m_render_device.create_shader(ShaderType::Vertex, fullscreen_quad_vs, "main", shader_macro_t(), m_vertex_shader))
			std::abort();
	}

	void FullscreenQuad::render()
	{
		m_render_device.bind_buffer(m_buffer);
		m_render_device.bind_shader(m_vertex_shader);
		m_render_device.draw(PrimitiveTopologyType::TriangleList, 6, 0);
	}
}