//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_entity.hpp"
#include "../core/ssa_bag.hpp"
#include "ssa_entity_handle.hpp"

namespace ssa
{
	// Forward declaration
	class EntityHandle;

	//! \brief Class that manages, creation / destruction of handles and garbage collection
	class ssa_export EntityFactory
	{
	public:
		//! \brief Creates a new instance and allocates a pool of entities
		EntityFactory();

		// @TODO : DESTROY ALL ENTITIES
		//! \brief Destructs all the associated entities
		~EntityFactory() = default;

		//! \brief Creates a new entity and returns a handle to it
		Entity& create_entity();

		//! \brief Returns the entity with the specified ID
		Entity& get_entity(Entity::id_t p_id);

		//! \brief Removes an entity from the active pool and unlinks all the components
		void remove_entity(Entity& p_entity);

	private:
		Bag m_entities;
	};

}