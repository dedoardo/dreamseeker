//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// C++ STD
#include <cstdlib>
#include <array>
#include <unordered_map>

// ssa
#include "ssa_component.hpp"
#include "ssa_entity_handle.hpp"
#include "../core/ssa_bag.hpp"

namespace ssa
{
	//! \brief Class that manages registration / creation of components and their linking to entities
	class ssa_export ComponentFactory
	{
		typedef std::size_t type_hash_t;
	public:
		//! \brief Creates a new instance of the class, but does not allocate memory till components are registered
		ComponentFactory();

		//! \brief Deallocates and unlinks all the components
		~ComponentFactory();

		//! \brief Constructs a component from the parameters and attaches it to the specified entity
		//! \param [in] p_entity_handle Handle of the entity the component will be attached to 
		//! \param [in] p_args Constructor arguments for the component
		template <typename component_t, typename ...ctor_args>
		Component::id_t attach_component(EntityHandle& p_entity_handle, ctor_args ...p_args);

		template <typename component_t>
		component_t& get_component(Component::id_t p_id);

		//! \brief Detaches a component from the specified entity
		//! \param [in] p_entity_handle Handle of the entity the component will be detached from
		template <typename component_t>
		void detach_component(EntityHandle& p_entity_handle);

		//! \brief Retrieves the internal buffer of components of the specified type ( user should not use this for any reason 
		//! \param [in] p_type DON'T CALL THIS METHOD
		//! \return DON'T CALL THIS METHOD
		Bag& get_components_all(Component::type_t p_type) { return *m_components[static_cast<std::size_t>(p_type)]; }

		//! \brief Returns the type ( index ) from the actual type, returns Component::max_component + 1 if does not exists
		template <typename component_t>
		Component::type_t get_type_from_component()const;

	private:
		std::array<Bag*, Component::max_component_number>	m_components;
		std::unordered_map<type_hash_t, Component::type_t>	m_types;
		std::size_t											m_last_type;
	};

	template <typename component_t, typename ...ctor_args>
	Component::id_t ComponentFactory::attach_component(EntityHandle& e, ctor_args ...p_args)
	{
		type_hash_t hash = typeid(component_t).hash_code();
		auto find_res = m_types.find(hash);
		Component::type_t new_type;

		if (find_res == m_types.end()) // Type has not been registered yet
		{
			new_type = m_last_type++;
			m_types[hash] = new_type; // Adding it to type register
			m_components[static_cast<std::size_t>(new_type)] = new Bag(sizeof(component_t), 10); // Creating new bag
		}
		else
			new_type = find_res->second;

		Component::id_t id = m_components[static_cast<std::size_t>(new_type)]->add_object<component_t>(p_args...);
		Component& new_component = m_components[static_cast<std::size_t>(new_type)]->get_object<Component>(id);

		// Filling out component's informations
		new_component.m_type = new_type;
		new_component.m_id = id;
		new_component.m_entity = &e.get();

		return id;
	}


	template <typename component_t>
	component_t& ComponentFactory::get_component(Component::id_t p_id)
	{
		return m_components[static_cast<std::size_t>(m_types[typeid(component_t).hash_code()])]->get_object<component_t>(p_id);
	}

	template <typename component_t>
	void ComponentFactory::detach_component(EntityHandle& p_entity_handle)
	{
		// @TODO : implement
	}

	template <typename component_t>
	Component::type_t ComponentFactory::get_type_from_component()const
	{
		auto find_res = m_types.find(typeid(component_t).hash_code());
		if (find_res == m_types.end())
			return Component::max_component_number + 1;
		return find_res->second;
	}
}