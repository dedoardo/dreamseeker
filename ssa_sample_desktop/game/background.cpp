#include "background.hpp"
#include <utility/ssa_loaders.hpp>
#include <iostream>
#include <string>

namespace
{
	const std::string ps_code
	{
	"Texture2D render_target : register(t0);"
	"SamplerState render_target_sampler : register(s0);"
	""
	"struct VS_OUTPUT"
	"{"
	"	float4 Position : SV_POSITION0;"
	"	float2 TexCoord : TEXCOORD0;"
	"};"
	"cbuffer Data"
	"{"
	"float time;"
	"}"

	"float3 rgb2hsv(float3 c)"
	"{"
	"float4 K = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);"
	"float4 p = lerp(float4(c.bg, K.wz), float4(c.gb, K.xy), step(c.b, c.g));"
	"float4 q = lerp(float4(p.xyw, c.r), float4(c.r, p.yzx), step(p.x, c.r));"

	"float d = q.x - min(q.w, q.y);"
	"float e = 1.0e-10;"
	"return float3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);"
	"}"

	"float3 hsv2rgb(float3 c)"
	"{"
	"float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);"
	"float3 p = abs(frac(c.xxx + K.xyz) * 6.0 - K.www);"
	"return c.z * lerp(K.xxx, saturate(p - K.xxx), c.y);"
	"}"

	"float4 main(VS_OUTPUT input) : SV_TARGET"
	"{"
	"float4 color = render_target.Sample(render_target_sampler, input.TexCoord);"
	"float3 hsv = rgb2hsv(color.xyz);"
	"hsv.x = (time % 360)- 180;"
	"return float4(hsv2rgb(hsv), 1.f);"
	"}"
	};
}

Background::Background() : 
m_render_device{ nullptr },
m_renderer2D{ nullptr },
m_fs_quad{ nullptr },
m_base_texture{ nullptr },
m_bg_ps{ nullptr },
m_base_sampler{ nullptr },
m_time{ 0 }
{

}

Background::~Background()
{
	for (auto& texture : m_stars)
		delete texture;

	delete m_fs_quad;
	delete m_base_texture;
	delete m_bg_ps;
	delete m_base_sampler;
}

void Background::init(ssa::RenderDevice& p_render_device, ssa::Renderer2D& p_renderer2D)
{
	m_render_device = &p_render_device;
	m_renderer2D = &p_renderer2D;

	for (unsigned int i = 0; i < 5; ++i)
	{
		ssa::loaded_data data;
		unsigned long width, height;
		std::string filename = "assets/star" + std::to_string(i + 1) + ".png";
		if (!ssa::load_png(filename, data, width, height))
		{
			std::cout << "Failed to load star data" << std::endl;
		}

		auto new_texture = new ssa::Texture(p_render_device);
		if (!new_texture->create(width, height, ssa::Format::RGBA8Unorm, false, &data[0]))
		{
			std::cout << "Failed to create star's textures" << std::endl;
		}

		m_stars.push_back(new_texture);
	}


	for (unsigned int i = 0; i < 5; ++i)
	{
		ssa::Sprite new_sprite;
		new_sprite.position.x = rand() % 1280;
		new_sprite.position.y = rand() % 720;
		new_sprite.set_texture(*m_stars[i]);

		m_star_sprites.push_back(new_sprite);
	}

	m_fs_quad = new ssa::FullscreenQuad(p_render_device);
	ssa::loaded_data data;
	unsigned long width, height;
	if (!ssa::load_png("assets/base_bg.png", data, width, height))
		std::cout << "Failed to load star data" << std::endl;

	m_base_texture = new ssa::Texture(p_render_device);
	if (!m_base_texture->create(width, height, ssa::Format::RGBA8Unorm, false, &data[0]))
	{
		std::cout << "Failed to create background shader" << std::endl;
	}

	m_bg_ps = new ssa::Shader(p_render_device);
	if (!m_bg_ps->create(ssa::Shader::Type::Pixel, ps_code, "main", ssa::shader_macro_t()))
		std::cout << "Failed to create background shader" << std::endl;

	m_base_sampler = new ssa::Sampler(p_render_device);
	if (!m_base_sampler->create(ssa::SamplerFilterType::Anisotropic))
		std::cout << "Failed to create background sampler" << std::endl;
}
void Background::update(unsigned int p_milliseconds)
{
	m_time += static_cast<float>(p_milliseconds) / 10000.f;
}

void Background::render()
{
	m_render_device->bind_shader(*m_bg_ps);
	m_render_device->set_texture(*m_bg_ps, "render_target", *m_base_texture);
	m_render_device->set_sampler(*m_bg_ps, "render_target_sampler", *m_base_sampler);
	m_render_device->set_value(*m_bg_ps, "time", &m_time, sizeof(m_time));

	m_fs_quad->render();

	//m_render_device->unbind_shader(ssa::Shader::Type::Pixel);

	m_renderer2D->begin(ssa::Renderer2D::SortMode::None);
	for (auto& star : m_star_sprites)
		m_renderer2D->render(star);
	m_renderer2D->end();
}