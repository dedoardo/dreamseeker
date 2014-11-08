#pragma once

#include <stegosaurus.hpp>

struct GameContext
{
	GameContext(ssa::RenderDevice& p_render_device, ssa::Renderer2D& p_renderer2D, ssa::Window& p_window, ssa::Texture& p_render_window) :
	render_device{ p_render_device },
	renderer2D{ p_renderer2D },
	window{ p_window },
	render_window{ p_render_window } { } 

	ssa::RenderDevice& render_device;
	ssa::Renderer2D& renderer2D;
	ssa::Window& window;
	ssa::Texture& render_window;
};