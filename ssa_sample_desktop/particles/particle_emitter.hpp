#pragma once

#include <stegosaurus.hpp>

class Particle : public ssa::Sprite
{
public :
	float lifetime;
	float current_life;
	ssa::float2 velocity;
	float speed;
};

class ParticleEmitter
{
public :
	ParticleEmitter(unsigned int p_capacity);
	~ParticleEmitter();

	void update(unsigned int p_milliseconds);
	void render(ssa::Renderer2D& p_renderer);

	void emit();

public :
	float		 emission_rate;
	ssa::Texture* texture;
	ssa::float4   texture_rect;
	ssa::float4	  start_color;
	ssa::float4	  end_color;
	float		  lifetime;
	ssa::float2   position;
	ssa::float2   start_velocity;
	ssa::float2   randomizer_velocity;
	float   start_speed;
	float   randomizer_speed;
	ssa::float2   start_scale;
	ssa::float2	  end_scale;
	ssa::float2   randomizer_scale;

private :
	unsigned int m_first_alive;
	unsigned int m_first_dead;
	unsigned int m_capacity;
	bool		 m_full;
	float		 m_accumulated_time;
	std::vector<Particle> m_sprite_buffer;
};