//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <entity/ssa_entity.hpp>

namespace ssa
{
	Entity::Entity() :
		ref_count{ 0 },
		id{ 0 }
	{
		// ODIO Visual Studio, default per i puntatori non e' 0x0000, ma 0x0c0c0c0 o qualche porcata simile , neanche gargabe
		std::memset(m_components, 0, sizeof(Component*)* Component::max_component_number);
	}

	Entity::~Entity()
	{
	}

	void Entity::add_component(Component* c, Component::type_t type)
	{
		//TODO when manager manager is ready
		m_components[type] = c;
	}

	bool Entity::has_component(Component::type_t p_type)
	{
		if (m_components[static_cast<std::size_t>(p_type)] == nullptr)
			return false;
		return true;
	}
}