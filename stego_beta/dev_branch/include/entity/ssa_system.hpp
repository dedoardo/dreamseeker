//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// C++ STD
#include <bitset>

// C++ STD
#include "ssa_component.hpp"

namespace ssa
{
	// Forward declarations
	class EntityHandle;
	class ComponentFactory;

	class ssa_export System
	{
		friend class SystemLooper;
	public:
		System() : m_enabled{ false } { } 
		~System() = default;

		template <typename component_t>
		void register_component();

		template <typename component_t>
		void unregister_component();

		template <typename component_t>
		bool is_component_registered();

		bool is_component_registered(Component::id_t p_id)const { return m_registered_components[static_cast<std::size_t>(p_id)]; }

		const std::bitset<Component::max_component_number>& get_registered_all()const { return m_registered_components; }

		virtual void preprocess() = 0;
		virtual void process(EntityHandle& p_next_entity) = 0;
		virtual void finalize() = 0;

		void enable() { m_enabled = true; }
		void disable() { m_enabled = false; }
		bool is_enabled()const { return m_enabled; }

	protected:
		bool			m_enabled;
		std::bitset<Component::max_component_number> m_registered_components;
	
	private :
		ComponentFactory* m_component_factory;
	};

	template <typename component_t>
	void System::register_component()
	{
		m_registered_components.set(static_cast<std::size_t>(m_component_factory->get_type_from_component<component_t>()), true);
	}

	template <typename component_t>
	void System::unregister_component()
	{
		m_registered_components.set(m_component_factory->get_type_from_component<component_t>(), false);
	}

	template <typename component_t>
	bool System::is_component_registered()
	{
		return m_registered_components[m_component_factory->get_type_from_component<component_t>()];
	}
}