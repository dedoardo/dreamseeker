//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <entity/ssa_system_looper.hpp>

// ssa
#include <entity/ssa_entity_factory.hpp>
#include <entity/ssa_component_factory.hpp>

namespace ssa
{
	SystemLooper::SystemLooper(EntityFactory& p_entity_factory,
		ComponentFactory& p_component_factory) :
		m_entity_factory{ &p_entity_factory },
		m_component_factory{ &p_component_factory }
	{

	}

	SystemLooper::~SystemLooper()
	{

	}

	void SystemLooper::process()
	{
		for (auto& system : m_systems)
		{
			system->preprocess();

			const auto& registered = system->get_registered_all();

			// Finding first registered component
			Component::type_t first_type = Component::max_component_number + 1;
			for (unsigned int i = 0; i < Component::max_component_number; ++i)
			{
				if (system->is_component_registered(i))
				{
					first_type = i;
					break;
				}
			}

			if (first_type == Component::max_component_number + 1)
				continue; // No components registered

			// Getting bag of components
			auto& first_bag = m_component_factory->get_components_all(first_type);

			// ""Iterating"" 
			uint8_t* data = first_bag.get_data_ptr();
			for (unsigned int i = 0; i < first_bag.get_last_element_pos(); ++i)
			{
				Component* component{ reinterpret_cast<Component*>(data + i * first_bag.get_element_size()) };
				if (component->get_entity() == nullptr) // Current spot is empty
					continue;

				Entity* next_entity{ component->get_entity() };

				bool valid{ true };
				for (unsigned int c = 0; c < Component::max_component_number; ++c)
				{
					if (registered[c] && !next_entity->has_component(c))
						valid = false;
				}

				if (valid)
					system->process(EntityHandle(*next_entity, *m_component_factory));
			}

			system->finalize();
		}
	}
}