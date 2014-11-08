#include "AABB.hpp"

bool AABB::collides_with(AABB& p_other)
{
	return !(p_other.get_position().x > m_position.x + m_size.x
		|| p_other.get_position().x + p_other.get_size().x < m_position.x
		|| p_other.get_position().y - p_other.get_size().y > m_position.y
		|| p_other.get_position().y < m_position.y + m_size.y);
}

bool AABB::collides_bottom(AABB& p_other)
{
	return !(p_other.get_position().y < m_position.y + m_size.y);
}

bool AABB::collides_top(AABB& p_other)
{
	return !(p_other.get_position().y - p_other.get_size().y > m_position.y);
}

bool AABB::collides_right(AABB& p_other)
{
	return !(p_other.get_position().x > m_position.x + m_size.x);
}

bool AABB::collides_left(AABB& p_other)
{
	return !(p_other.get_position().x + p_other.get_size().x < m_position.x);
}