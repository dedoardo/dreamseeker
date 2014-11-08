#pragma once

// Stegosaurus
#include <stegosaurus.hpp>

#include "../map/map_structs.hpp"
#include "player_component.hpp"

using namespace ssa;

class PhysicsSystem
{
public:
	PhysicsSystem()
		: m_delta_time{ 0 }
	, is_active{ false }
	{}

	void preprocess();
	void process(Object& p_object);
	void postprocess();

	void set_delta_time(float p_delta) { m_delta_time = p_delta; }

	void set_player(PlayerComponent* p_player) { m_player = p_player; }
	void set_gravity(float p_gravity) { m_gravity = p_gravity; }
	void set_active(bool p_active) { is_active = p_active; }
	void check(Object& p_object);

	float2 player_input;
private:
	bool check_collision(Object& p_object);
	bool check_top_collision(Object& p_object);
	bool check_bottom_collision(Object& p_object);
	bool check_right_collision(Object& p_object);
	bool check_left_collision(Object& p_object);

	float m_gravity;
	bool is_active;


	float m_delta_time;

	PlayerComponent* m_player;
	float2 _next_velocity;
};