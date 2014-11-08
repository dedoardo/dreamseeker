//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_component.hpp"

// C++ STD
#include <cstdint>
#include <array>

namespace ssa
{
	//! \brief 
	class ssa_export Entity
	{
	public:
		typedef std::uint64_t id_t;

		Entity();
		~Entity();

		template <typename component_t>
		component_t& get_component(Component::type_t p_type);

		void add_component(Component* c, Component::type_t type);

		bool has_component(Component::type_t type);

		std::uint64_t ref_count;
		id_t id;

	private:
		Component* m_components[Component::max_component_number];
	};

	template <typename component_t>
	component_t& Entity::get_component(Component::type_t p_type)
	{
		return *(static_cast<component_t*>(m_components[static_cast<std::size_t>(p_type)]));
	}
}