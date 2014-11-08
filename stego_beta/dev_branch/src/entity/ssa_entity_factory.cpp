//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <entity/ssa_entity_factory.hpp>

namespace ssa
{
	EntityFactory::EntityFactory() :
		m_entities(sizeof(Entity), 50)
	{
	}

	Entity& EntityFactory::create_entity()
	{
		Entity::id_t id = m_entities.add_object<Entity>();
		Entity& new_entity = m_entities.get_object<Entity>(id);
		new_entity.id = id;
		return new_entity;
	}

	Entity& EntityFactory::get_entity(Entity::id_t p_id)
	{
		return m_entities.get_object<Entity>(p_id);
	}

	void EntityFactory::remove_entity(Entity& p_entity)
	{
		p_entity.ref_count--;
		if (p_entity.ref_count <= 0)
			m_entities.recycle(p_entity.id);
	}
}