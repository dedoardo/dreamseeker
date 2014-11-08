//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <graphics/effects/ssa_pp_horizontal_blur_pass.hpp>

// ssa
#include <graphics/ssa_render_device.hpp>
#include <graphics/ssa_texture.hpp>

namespace ssa
{
	namespace
	{
		const std::string horizontal_blur_ps_code
		{
			"Texture2D render_target : register(t0);"
			"SamplerState render_target_sampler : register(s0);"
			"struct VS_OUTPUT"
			"{"
			"float4 Position : SV_POSITION0;"
			"float2 TexCoord : TEXCOORD0;"
			"};"
			"float4 main(VS_OUTPUT input) : SV_TARGET"
			"{"
			"float4 final_color = float4(0.f, 0.f, 0.f, 0.f);"

			"float2 texcoord;"
			"texcoord.x = input.TexCoord.x + (1.0 / float(1920)) / 2.0;"
			"texcoord.y = input.TexCoord.y + (1.0 / float(1080)) / 2.0;"

			"float blur_size = 1.0 / 1920;"

			"final_color += render_target.Sample(render_target_sampler, float2(input.TexCoord.x - 4.0 * blur_size, input.TexCoord.y)) * 0.05;"

			"final_color += render_target.Sample(render_target_sampler, float2(input.TexCoord.x - 3.0 * blur_size, input.TexCoord.y)) * 0.09;"

			"final_color += render_target.Sample(render_target_sampler, float2(input.TexCoord.x - 2.0 * blur_size, input.TexCoord.y)) * 0.12;"

			"final_color += render_target.Sample(render_target_sampler, float2(input.TexCoord.x - blur_size, input.TexCoord.y)) * 0.15;"

			"final_color += render_target.Sample(render_target_sampler, input.TexCoord) * 0.16;"

			"final_color += render_target.Sample(render_target_sampler, float2(input.TexCoord.x + blur_size, input.TexCoord.y)) * 0.15;"

			"final_color += render_target.Sample(render_target_sampler, float2(input.TexCoord.x + 2.0 * blur_size, input.TexCoord.y)) * 0.12;"

			"final_color += render_target.Sample(render_target_sampler, float2(input.TexCoord.x + 3.0 * blur_size, input.TexCoord.y)) * 0.09;"

			"final_color += render_target.Sample(render_target_sampler, float2(input.TexCoord.x + 4.0 * blur_size, input.TexCoord.y)) * 0.05;"

			"return final_color;"
		"}"
		};
	}

	HorizontalBlurPass::HorizontalBlurPass(RenderDevice& p_render_device) :
		PostProcessPass{ p_render_device }
	{
		if (!p_render_device.create_shader(Shader::Type::Pixel, horizontal_blur_ps_code, "main", Shader::macro_t(), m_shader))
			std::abort();
	}

	void HorizontalBlurPass::set_params()
	{

	}
}