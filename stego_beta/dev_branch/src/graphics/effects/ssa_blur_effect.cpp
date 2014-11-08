//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <graphics/effects/ssa_blur_effect.hpp>

// ssa
#include <graphics/ssa_render_device.hpp>

namespace ssa
{
	BlurEffect::BlurEffect(RenderDevice& p_render_device) :
		m_render_device{ p_render_device },
		m_horizontal{ p_render_device },
		m_vertical{ p_render_device },
		m_buffer{ p_render_device },
		m_quad{ p_render_device },
		m_sampler{ p_render_device }
	{
		if (!m_render_device.create_texture(1920, 1080, Format::RGBA32Float, false, nullptr, m_buffer))
			std::abort();

		if (!m_render_device.create_sampler(SamplerFilterType::Linear, m_sampler))
			std::abort();
	}

	void BlurEffect::process(Texture& p_input_target)
	{
		m_render_device.clear(m_buffer, float4(0.f, 0.f, 0.f, 0.f));

		// Setting optional buffer
		m_render_device.push_target(m_buffer, false);
	
		// Binding horizontal blur shader
		m_render_device.bind_shader(m_horizontal.get_shader());

		// Setting input target for first pass
		m_render_device.set_texture(m_horizontal.get_shader(), "render_target", p_input_target);
		m_render_device.set_sampler(m_horizontal.get_shader(), "render_target_sampler", m_sampler);

		//// Rendering fullscreen quad
		m_quad.render();

		//// Removing optional buffer
		m_render_device.pop_targets();

		//// Binding vertical blur shader
		m_render_device.bind_shader(m_vertical.get_shader());

		//// Using it as shader resource
		m_render_device.set_texture(m_vertical.get_shader(), "render_target", m_buffer);
		m_render_device.set_sampler(m_vertical.get_shader(), "render_target_sampler", m_sampler);

		// Rendering to output target
		m_quad.render();
	}
}