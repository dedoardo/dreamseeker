///! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_hardware_layer.hpp"
#include "ssa_device.hpp"
#include "ssa_winrt_device.hpp"

// C++ STD
#include <vector>

namespace ssa
{
	class WinRTHardwareLayer : public HardwareLayer
	{
	public:

		//! Initialize hardware layer
		bool init();

	private:
		//! Creates internal virtual device representing phone's input
		void create_devices();

		std::vector<WinRTDevice> _winrt_devices;
	};
}