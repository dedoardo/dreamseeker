//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <entity/ssa_component_factory.hpp>

namespace ssa
{
	ComponentFactory::ComponentFactory() :
		m_last_type{ 0 }
	{
		for (auto& bag : m_components)
			bag = nullptr;
	}

	ComponentFactory::~ComponentFactory()
	{

	}
}