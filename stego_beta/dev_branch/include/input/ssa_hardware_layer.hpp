//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_device.hpp"
#include "ssa_platform_device.hpp"

namespace ssa
{
	class HardwareLayer
	{
	public:
		virtual const size_t							get_type_count(Device::Type p_type);

		virtual Device*									find_device(Device::Type p_type, size_t p_index);

		virtual std::vector<Device>&					get_devices() { return m_devices; };

		virtual std::vector<PlatformDevice*>&			get_platform_devices() { return m_platform_devices; };

	protected:
		std::vector<Device>								m_devices;

		std::vector<PlatformDevice*>					m_platform_devices;

	};
}