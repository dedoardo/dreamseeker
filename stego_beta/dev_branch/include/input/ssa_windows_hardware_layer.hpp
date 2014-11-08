//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_device.hpp"
#include "ssa_hardware_layer.hpp"
#include "ssa_windows_device.hpp"
#include <window\ssa_window.hpp>

// DInput
#include <dinput.h>

// C++ STD
#include <vector>
#include <string>

namespace ssa
{
	class WindowsHardwareLayer : public HardwareLayer
	{
	public:
		WindowsHardwareLayer();
		~WindowsHardwareLayer();

		void										init(Window& p_window);

		void										update_devices();

	private:

		void										create_devices(HWND& p_window);

		void										configure_devices(Window& p_window);

		IDirectInput8*								m_direct_input;

		std::vector<WindowsDevice>					_windows_devices;
	};
}