//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_component.hpp"
#include "ssa_entity.hpp"

namespace ssa
{
	// Forward declaration
	class ComponentFactory;

	class ssa_export EntityHandle
	{
	public:
		EntityHandle(Entity& e, ComponentFactory& p_component_factory);
		EntityHandle(const EntityHandle& p_other);
		EntityHandle& operator=(const EntityHandle& p_other);
		~EntityHandle();

		Entity& get() { return *m_entity; }

		Entity::id_t get_id()const { return m_entity->id; }

		template <typename component_t>
		component_t& get_component();

		template <typename component_t, typename ...ctor_args_t>
		void attach_component(ctor_args_t ...p_ctor_args);

	private:
		ComponentFactory*	m_component_factory;
		Entity*				m_entity;
	};

	template <typename component_t>
	component_t& EntityHandle::get_component()
	{
		return m_entity->get_component<component_t>(m_component_factory->get_type_from_component<component_t>());
	}

	template <typename component_t, typename ...ctor_args_t>
	void EntityHandle::attach_component(ctor_args_t ...p_ctor_args)
	{
		Component::id_t new_component_id = m_component_factory->attach_component<component_t>(*this, p_ctor_args...);
		m_entity->add_component(&m_component_factory->get_component<component_t>(new_component_id), m_component_factory->get_type_from_component<component_t>());
	}
}