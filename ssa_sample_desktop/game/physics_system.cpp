#include "physics_system.hpp"

void PhysicsSystem::preprocess()
{
	if (m_player == nullptr)
		return;
	m_player->velocity.x = player_input.x;
	if (m_player->velocity.y == 0.f)
		m_player->velocity.y = player_input.y;

	m_player->velocity.y += m_gravity * m_delta_time / 1000.f;

	_next_velocity = m_player->velocity;
}

void PhysicsSystem::process(Object& p_object)
{
	if (m_player == nullptr)
		return;

	if (check_collision(p_object))
	{ // Player is colliding with this object
		// TODO check object's properties
		if (check_bottom_collision(p_object)/* && m_player->velocity.y > 0*/)
		{
			_next_velocity.y = 0.f;
		}
		if (check_top_collision(p_object)/* && m_player->velocity.y < 0*/)
		{
			_next_velocity.y = m_gravity * m_delta_time / 1000.f;
		}
		if (check_right_collision(p_object)/* && m_player->velocity.x > 0*/)
		{
			_next_velocity.x = 0.f;
		}
		if (check_left_collision(p_object)/* && m_player->velocity.x < 0*/)
		{
			_next_velocity.x = 0.f;
		}
	}
}

void PhysicsSystem::postprocess()
{
	m_player->velocity = _next_velocity;
	m_player->position.x += m_player->velocity.x;
	m_player->position.y += m_player->velocity.y;
}

bool PhysicsSystem::check_collision(Object& p_object)
{
	float k = m_player->position.y + m_player->velocity.y + m_player->size.y;
	return !(p_object.position.x > m_player->position.x + m_player->velocity.x + m_player->size.x
		|| p_object.position.x + p_object.size.x < m_player->position.x + m_player->velocity.x
		|| p_object.position.y + p_object.size.y < m_player->position.y + m_player->velocity.y
		|| p_object.position.y > m_player->position.y + m_player->velocity.y + m_player->size.y);
}

bool PhysicsSystem::check_left_collision(Object& p_object)
{
	return ((m_player->position.x + m_player->velocity.x > p_object.position.x) &&
		(m_player->position.x + m_player->velocity.x + m_player->size.x > p_object.position.x + p_object.size.x) &&
		!(p_object.position.y > m_player->position.y + m_player->size.y));
}

bool PhysicsSystem::check_right_collision(Object& p_object)
{
	return ((m_player->position.x + m_player->velocity.x < p_object.position.x) &&
		(m_player->position.x + m_player->velocity.x + m_player->size.x < p_object.position.x + p_object.size.x) &&
		!(p_object.position.y > m_player->position.y + m_player->size.y));
}

bool PhysicsSystem::check_bottom_collision(Object& p_object)
{
	return ((m_player->position.y + m_player->velocity.y < p_object.position.y) &&
		(m_player->position.y + m_player->velocity.y + m_player->size.y < p_object.position.y + p_object.size.y) &&
		!(p_object.position.x > m_player->position.x + m_player->size.x) &&
		!(p_object.position.x + p_object.size.x < m_player->position.x));
}

bool PhysicsSystem::check_top_collision(Object& p_object)
{
	return ((m_player->position.y + m_player->velocity.y > p_object.position.y) &&
		(m_player->position.y + m_player->velocity.y + m_player->size.y > p_object.position.y + p_object.size.y) &&
		!(p_object.position.x > m_player->position.x + m_player->size.x) &&
		!(p_object.position.x + p_object.size.x < m_player->position.x));
}
