//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include <graphics/ssa_post_process_effect.hpp>
#include <graphics/effects/ssa_pp_horizontal_blur_pass.hpp>
#include <graphics/effects/ssa_pp_vertical_blur_pass.hpp>
#include <graphics/ssa_texture.hpp>
#include <graphics/ssa_fullscreen_quad.hpp>
#include <graphics/ssa_sampler.hpp>

namespace ssa
{
	class BlurEffect : public PostProcessEffect
	{
	public : 
		BlurEffect(RenderDevice& p_render_device);
		void process(Texture& p_input_target)override;

	private :
		RenderDevice&		m_render_device;
		HorizontalBlurPass	m_horizontal;
		VerticalBlurPass	m_vertical;
		Texture				m_buffer;
		FullscreenQuad		m_quad;
		Sampler				m_sampler;
	};
}