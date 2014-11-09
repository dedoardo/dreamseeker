#pragma once

#include <stegosaurus.hpp>

class Background
{
public :
	Background();
	~Background();

	void init(ssa::RenderDevice& p_render_device, ssa::Renderer2D& p_renderer2D);

	void update(unsigned int p_milliseconds);
	void render();

private :
	ssa::RenderDevice* m_render_device;
	ssa::Renderer2D* m_renderer2D;
	std::vector<ssa::Texture*> m_stars;
	std::vector<ssa::Sprite> m_star_sprites;

	ssa::FullscreenQuad*  m_fs_quad;
	ssa::Texture*		  m_base_texture;
	ssa::Shader*		  m_bg_ps;
	ssa::Sampler*		  m_base_sampler;
	float m_time;
};