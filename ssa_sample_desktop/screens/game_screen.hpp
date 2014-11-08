#pragma once

#include "screen.hpp"
#include "game_context.hpp"
#include "../game/player_component.hpp"
#include "../map/map_structs.hpp"
#include "../game/physics_system.hpp"
#include "../particles/particle_emitter.hpp"
#include "../particles/particle_system.hpp"

#include <stegosaurus.hpp>
#include <graphics/effects/ssa_blur_effect.hpp>

class GameScreen : public Screen<GameContext>
{
public :
	GameScreen(ScreenStack<GameContext>& p_screen_machine);
	~GameScreen();

	void on_activation(GameContext& p_context)override;
	void on_deactivation(GameContext& p_context)override;

	void on_key_down(Window::Event, Window&, unsigned int p_code, unsigned int);
	void on_key_up(Window::Event, Window&, unsigned int p_code, unsigned int);

	void update(GameContext& p_context, unsigned int p_milliseconds, bool p_is_hidden)override;
	void render(GameContext& p_context, unsigned int p_milliseconds)override;

	void on_keyboard_event(const ssa::Device& p_device, const std::vector<ssa::Control*>& p_controls);

private :
	PhysicsSystem                           m_physics;
	PlayerComponent                         m_player_component;
	Map							m_map;
	GameContext*				m_context;
	std::vector<ssa::Texture>	m_texture_atlases;
	std::vector<ssa::Sprite>	m_sprite_buffer;

	ParticleSystem				m_particle_system;
	ParticleEmitter				m_player_emitter;
	Texture*					m_particle_texture;
	Sprite						m_player_sprite;
	Texture*					m_player_texture;
	BlurEffect*					m_base_blur;
	Blender*					m_blender;
};