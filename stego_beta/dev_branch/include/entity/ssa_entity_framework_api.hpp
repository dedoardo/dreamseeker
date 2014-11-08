//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_entity_factory.hpp"
#include "ssa_component_factory.hpp"
#include "ssa_system_looper.hpp"

namespace ssa
{
	//! \brief Entry point of API, most of the EF-related call should pass here, for the most part it simply manages
	//!		an EntityFactory, ComponentFactory, SystemLooper for you and provides some easier methods since it has access to all three of them
	class ssa_export EntityFrameworkAPI
	{
	public:
		//! \brief Constructs a new instance of the API and the sub-components [ all the allocation happens here ]
		EntityFrameworkAPI();

		//! \brief Destructs all the subcomponents and deallocates all the memory
		~EntityFrameworkAPI();

		// ===== ENTITY-RELATED METHODS =====
		//! \brief Retrieves a new entity from the pool, this returns a fresh entity not linked to any components
		//! \return Handle to the new entity
		EntityHandle create_entity();

		//! \brief Retrivies the handle from the entity ID ( user should not keep an entity ID around tho )
		//! \param [in] p_id Id of the entity
		//! \return Handle to an already existing entity
		EntityHandle get_entity(Entity::id_t p_id);

		//! \brief Retrieves a reference to the internal factory used by the EntityFrameworkAPI
		//! \return Reference to the factory
		EntityFactory& get_entity_factory() { return m_entity_factory; }

		// ===== COMPONENT-RELATED METHODS =====
		//! \brief Retrieves a reference to the internal factory used by the EntityFrameworkAPI
		//! \return Reference to the factory
		ComponentFactory& get_component_factory() { return m_component_factory; }

		// ===== SYSTEM-RELATED METHODS ====
		//! \brief Adds a new system to the list of systems that will process entities
		template <typename system_t, typename ...ctor_args_t>
		void add_system(ctor_args_t ...p_ctor_args);

		//! \brief Removes an already added system from the list
		//! \param [in] p_system System to be removed
		template <typename system_t>
		void remove_system();

		template <typename system_t>
		system_t& get_system();

		//! \brief Core of the EF, loops through all the systems and calls process() on the entities that have the 
		//!		components matching the system's ones
		void process();

		//! \brief Retrieves a reference to the internal system looper use by the EntityFrameworkAPI
		SystemLooper& get_system_looper() { return m_system_looper; }

	private:
		EntityFactory		m_entity_factory;
		ComponentFactory	m_component_factory;
		SystemLooper		m_system_looper;
	};

	template <typename system_t, typename ...ctor_args_t>
	void EntityFrameworkAPI::add_system(ctor_args_t ...p_ctor_args)
	{
		m_system_looper.add_system<system_t>(p_ctor_args...);
	}

	template <typename system_t>
	void EntityFrameworkAPI::remove_system()
	{
		m_system_looper.remove_system<system_t>();
	}

	template <typename system_t>
	system_t& EntityFrameworkAPI::get_system()
	{
		return m_system_looper.get_system<system_t>();
	}
}