#include "particle_system.hpp"
#include "particle_emitter.hpp"
#include <stegosaurus.hpp>

#include <iostream>

namespace
{
	const std::string ps_code
	{
	"Texture2D render_target : register(t0);"
	"SamplerState render_target_sampler : register(s0);"

	"struct VS_OUTPUT"
	"{"
	"	float4 Position : SV_POSITION0;"
	"	float2 TexCoord : TEXCOORD0;"
	"};"

	"float4 main(VS_OUTPUT input) : SV_TARGET"
	"{"
	"return render_target.Sample(render_target_sampler, input.TexCoord);"
	"}"
	};
}

ParticleSystem::ParticleSystem() : 
m_offscreen_rt{ nullptr },
m_blender{ nullptr },
m_fs_quad{ nullptr }
{

}

ParticleSystem::~ParticleSystem()
{
	delete m_offscreen_rt;
	delete m_sampler;
	delete m_blender;
	delete m_fs_quad;
	delete m_alpha_blender;
	delete m_shader;
}

void ParticleSystem::init(ssa::RenderDevice& p_render_device, ssa::Renderer2D& p_renderer)
{
	m_render_device = &p_render_device;
	renderer = &p_renderer;

	m_offscreen_rt = new ssa::Texture(p_render_device);
	m_sampler = new ssa::Sampler(p_render_device);
	m_blender = new ssa::Blender(p_render_device);
	m_alpha_blender = new ssa::Blender(p_render_device);
	m_fs_quad = new ssa::FullscreenQuad(p_render_device);
	m_shader = new ssa::Shader(p_render_device);

	if (!m_offscreen_rt->create(1280, 720, ssa::Texture::Format::RGBA32Float, false, nullptr))
		std::cout << "Failed to create offscreen render target for particle system " << std::endl;
	if (!m_blender->create(ssa::BlendType::Additive))
		std::cout << "Failed to create blender for particle system";
	if (!m_alpha_blender->create(ssa::BlendType::Alpha))
		std::cout << "Failed to create alpha blender for particle system";
	if (!m_shader->create(ssa::Shader::Type::Pixel, ps_code, "main", ssa::shader_macro_t()))
		std::cout << "Failed to create shader for particle system";
	if (!m_sampler->create(ssa::SamplerFilterType::Anisotropic))
		std::cout << "Failed to create sampler for particle system";
}

void ParticleSystem::update(unsigned int p_milliseconds)
{
	for (auto& emitter : emitters)
		emitter->update(p_milliseconds);
}

void ParticleSystem::render(unsigned int p_milliseconds)
{
	// Rendering everything on offscreen render target
	m_render_device->push_target(*m_offscreen_rt, false);
	m_render_device->clear(*m_offscreen_rt, ssa::float4(0.f, 0.f, 0.f, 0.f));
	m_render_device->bind_blender(*m_blender);
	for (auto& emitter : emitters)
		emitter->render(*renderer);
	m_render_device->unbind_blender();
	m_render_device->pop_targets();

	// Finally rendering all onto the final one
	m_render_device->bind_shader(*m_shader);
	m_render_device->set_texture(*m_shader, "render_target", *m_offscreen_rt);
	m_render_device->set_sampler(*m_shader, "render_target_sampler", *m_sampler);

	m_render_device->bind_blender(*m_blender);

	m_fs_quad->render();

	m_render_device->unbind_blender();
}