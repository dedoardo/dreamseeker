//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <entity/ssa_entity_handle.hpp>
#include <entity/ssa_entity.hpp>

namespace ssa
{
	EntityHandle::EntityHandle(Entity& p_entity, ComponentFactory& p_component_factory) :
		m_entity{ &p_entity },
		m_component_factory{ &p_component_factory }
	{
		++m_entity->ref_count;
	}

	EntityHandle::~EntityHandle()
	{
		--m_entity->ref_count;
	}

	EntityHandle::EntityHandle(const EntityHandle& p_other) : 
		m_entity{ p_other.m_entity },
		m_component_factory{ p_other.m_component_factory }
	{
		++m_entity->ref_count;
	}

	EntityHandle& EntityHandle::operator=(const EntityHandle& p_other)
	{
		--m_entity->ref_count;
		m_entity = p_other.m_entity;
		++m_entity->ref_count;

		return *this;
	}
}