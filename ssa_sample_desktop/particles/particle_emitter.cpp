#include "particle_emitter.hpp"
#include <random>

ParticleEmitter::ParticleEmitter(unsigned int p_capacity) :
m_capacity{ p_capacity },
m_accumulated_time{ 0 },
m_first_alive{ 0 },
m_first_dead{ 0 },
m_full{ false },

texture{ nullptr },
start_color{ ssa::float4(1.f, 1.f, 1.f, 1.f) },
end_color{ ssa::float4(1.f, 1.f, 1.f, 1.f) },
lifetime{ 1.f },
position{ ssa::float2(0.f, 0.f) },
start_velocity{ ssa::float2(1.f, 1.f) },
randomizer_velocity{ ssa::float2(0.f, 0.f) },
start_speed{ 10 },
randomizer_speed{ 0 },
start_scale{ 1.f },
end_scale{ 1.f },
randomizer_scale{ 0.f }
{
	m_sprite_buffer.resize(p_capacity);

	// Magic seed
	srand(66666);
}

ParticleEmitter::~ParticleEmitter()
{
}

void ParticleEmitter::update(unsigned int p_milliseconds)
{
	float elapsed_ms = static_cast<float>(p_milliseconds) / 1000.f;
	m_accumulated_time += elapsed_ms;

	while (m_accumulated_time >= emission_rate)
	{
		emit();
		m_accumulated_time -= emission_rate;
	}

	unsigned int count{ m_first_alive };
	while (count != m_first_dead)
	{
		m_sprite_buffer[count].current_life += elapsed_ms;

		m_sprite_buffer[count].position.x += m_sprite_buffer[count].velocity.x * elapsed_ms * m_sprite_buffer[count].speed;
		m_sprite_buffer[count].position.y += m_sprite_buffer[count].velocity.y * elapsed_ms * m_sprite_buffer[count].speed;
		count = (count + 1) % m_capacity;
	}

	count = { m_first_alive };
	while (count != m_first_dead && m_sprite_buffer[count].current_life >= lifetime)
	{
		m_first_alive = (m_first_alive + 1) % m_capacity;
		count = (count + 1) % m_capacity;
	}
}

void ParticleEmitter::render(ssa::Renderer2D& p_renderer)
{
	p_renderer.begin(ssa::Renderer2D::SortMode::None);
	unsigned int count{ m_first_alive };
	while (count != m_first_dead)
	{
		p_renderer.render(m_sprite_buffer[count]);
		count = (count + 1) % m_capacity;
	}
	p_renderer.end();
}

void ParticleEmitter::emit()
{
	if (m_full)
		return;

	if (((m_first_dead + 1) % m_capacity) == m_first_alive)
	{
		m_full = false;
	}

	m_sprite_buffer[m_first_dead].set_color(start_color);
	m_sprite_buffer[m_first_dead].position = position;
	if (texture != nullptr)
		m_sprite_buffer[m_first_dead].set_texture(*texture);

	m_sprite_buffer[m_first_dead].lifetime = lifetime;
	m_sprite_buffer[m_first_dead].current_life = 0;
	m_sprite_buffer[m_first_dead].velocity = start_velocity;
	m_sprite_buffer[m_first_dead].velocity.x += randomizer_velocity.x * rand() / RAND_MAX;
	m_sprite_buffer[m_first_dead].velocity.y += randomizer_velocity.y * rand() / RAND_MAX;

	m_sprite_buffer[m_first_dead].speed = start_speed;
	m_sprite_buffer[m_first_dead].speed += randomizer_speed * rand() / RAND_MAX;

	m_sprite_buffer[m_first_dead].scale = start_scale;
	m_sprite_buffer[m_first_dead].scale.x += -randomizer_scale.x / 2.f + randomizer_scale.x * rand() / RAND_MAX;
	m_sprite_buffer[m_first_dead].scale.y += -randomizer_scale.y / 2.f + randomizer_scale.y * rand() / RAND_MAX;

	m_first_dead = (m_first_dead + 1) % m_capacity;
}