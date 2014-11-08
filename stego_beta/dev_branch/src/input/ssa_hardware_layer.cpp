//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <input/ssa_hardware_layer.hpp>

namespace ssa
{
	const size_t HardwareLayer::get_type_count(Device::Type p_type)
	{
		size_t count{ 0 };
		for (const auto& device : m_devices)
		{
			if (device.get_data().type == p_type)
				++count;
		}

		return count;
	}

	Device* HardwareLayer::find_device(Device::Type p_type, size_t p_index)
	{
		if (p_index >= get_type_count(p_type))
			return nullptr;

		size_t index{ 0 };

		for (auto& device : m_devices)
		{
			if (p_type == device.get_data().type)
			{
				if (index == p_index)
					return &device;
				++index;
			}
		}

		//! This is not supposed to be executed in any way
		return nullptr;
	}
}