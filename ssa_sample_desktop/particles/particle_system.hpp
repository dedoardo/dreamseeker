#pragma once

#include <vector>
#include <stegosaurus.hpp>

class ParticleEmitter;

class ParticleSystem
{
public :
	ParticleSystem();
	~ParticleSystem();

	void init(ssa::RenderDevice& p_render_device, ssa::Renderer2D& p_renderer);

	void update(unsigned int p_milliseconds);
	void render(unsigned int p_milliseconds);

	std::vector<ParticleEmitter*>   emitters;
	ssa::Renderer2D*				renderer;

private :
	ssa::RenderDevice* m_render_device;
	ssa::Texture* m_offscreen_rt;
	ssa::Sampler* m_sampler;
	ssa::Blender* m_blender;
	ssa::Blender* m_alpha_blender;
	ssa::Shader*  m_shader;
	ssa::FullscreenQuad* m_fs_quad;
};