//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <input/ssa_winrt_hardware_layer.hpp>

namespace ssa
{
	bool WinRTHardwareLayer::init()
	{
		create_devices();

		return true;
	}

	void WinRTHardwareLayer::create_devices()
	{
		Device::Data data;

		data.id = 0;
		data.name = L"Pointer1";
		data.product_name = L"WP_Pointer";
		data.type = Device::Type::Pointer;

		Device::Capabilities capabilities;

		capabilities.axis = 2;
		capabilities.buttons = 1;
		capabilities.dpads = 0;

		_winrt_devices.emplace_back();

		WinRTDevice& winrt_pointer_device = _winrt_devices.back();
		m_platform_devices.push_back(&_winrt_devices.back());

		m_devices.emplace_back(data, capabilities);
		m_devices.back().set_controls(winrt_pointer_device.get_controls());
	}
}