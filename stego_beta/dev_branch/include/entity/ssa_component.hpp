//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// C++ STD
#include <cstdint>
#include <bitset>

// ssa
#include "../core/ssa_platform.hpp"

namespace ssa
{
	// Forward declaration
	class Entity;

	class ssa_export Component
	{
		friend class ComponentFactory;
	public:
		const static std::uint64_t max_component_number{ 42 };

		typedef std::uint64_t id_t;
		typedef std::uint64_t type_t;

	public:
		Component() : m_type{ 0 }, m_entity{ nullptr }, m_id{ 0 } { } 

		// TODO ? dobbiamo togliere il link quando va out of scope ?
		~Component() = default;

		type_t get_type()const { return m_type; }
		Entity* get_entity()const { return m_entity; }
		id_t get_id()const { return m_id; }

	private:
		// Type of the component, can range from 0 to MAX_COMPONENT_NUMBER
		type_t  m_type;

		// Entity the Component is linked to 
		Entity* m_entity;

		id_t	m_id;
	};
}