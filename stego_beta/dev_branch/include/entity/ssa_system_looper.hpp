//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// Header
#include "ssa_system.hpp"

// C++ STD
#include <vector>
#include <functional>
#include <unordered_map>

namespace ssa
{
	class System;
	class EntityFactory;
	class ComponentFactory;

	class ssa_export SystemLooper
	{
	public:
		SystemLooper(EntityFactory& p_entity_factory,
			ComponentFactory& p_component_factory);
		~SystemLooper();

		void process();

		template <typename system_t, typename ...ctor_args>
		void add_system(ctor_args ...p_ctor_args);

		template <typename system_t>
		system_t& get_system();

		template <typename system_t>
		void remove_system();

		template <typename system_t>
		void is_running();

	private:
		std::unordered_map<std::size_t, std::size_t> m_type_map;
		std::vector<System*>						 m_systems;
		EntityFactory*								 m_entity_factory;
		ComponentFactory*							 m_component_factory;
	};

	template <typename system_t, typename ...ctor_args>
	void SystemLooper::add_system(ctor_args ...p_ctor_args)
	{
		m_systems.push_back(new system_t(p_ctor_args...));
		m_systems.back()->m_component_factory = m_component_factory;
		std::size_t index = m_systems.size();

		m_type_map.insert(std::make_pair(typeid(system_t).hash_code(), --index));
	}

	template <typename system_t>
	system_t& SystemLooper::get_system()
	{
		return *static_cast<system_t*>(m_systems[m_type_map[typeid(system_t).hash_code()]]);
	}

	template <typename system_t>
	void SystemLooper::remove_system()
	{
		delete m_systems[m_type_map[typeid(system_t).hash_code()]];
		m_systems.erase(m_systems.begin() + m_type_map[typeid(system_t).hash_code()]);
	}

	template <typename system_t>
	void SystemLooper::is_running()
	{
		get_system<system_t>().is_active();
	}
}