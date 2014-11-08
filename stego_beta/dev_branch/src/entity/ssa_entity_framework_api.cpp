//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <entity/ssa_entity_framework_api.hpp>

namespace ssa
{
	EntityFrameworkAPI::EntityFrameworkAPI() : 
		m_entity_factory{},
		m_component_factory{},
		m_system_looper{ m_entity_factory, m_component_factory }
	{

	}

	EntityFrameworkAPI::~EntityFrameworkAPI()
	{

	}

	// ===== ENTITY-RELATED METHODS =====
	EntityHandle EntityFrameworkAPI::create_entity()
	{
		return EntityHandle(m_entity_factory.create_entity(), m_component_factory);
	}

	EntityHandle EntityFrameworkAPI::get_entity(Entity::id_t p_id)
	{
		return EntityHandle(m_entity_factory.get_entity(p_id), m_component_factory);
	}

	void EntityFrameworkAPI::process()
	{
		m_system_looper.process();
	}
}