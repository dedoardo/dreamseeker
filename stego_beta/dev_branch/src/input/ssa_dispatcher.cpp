//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <input/ssa_dispatcher.hpp>

// ssa
#include <input/ssa_hardware_layer.hpp>
#include <input/ssa_context.hpp>

// C++ STD
#include <memory>
#include <cassert>
#include <array>
#include <limits>

namespace ssa
{
	Dispatcher::Dispatcher(HardwareLayer* p_hwl) :
		m_ihl{ p_hwl }
	{

	}

	Dispatcher::~Dispatcher()
	{
	}

	void Dispatcher::push_context(Context& _context)
	{
		m_context_stack.push_back(&_context);
	}

	void Dispatcher::pop_context()
	{
		m_context_stack.pop_back();
	}

	void Dispatcher::dispatch()
	{
		std::vector<Device>& devices = m_ihl->get_devices();
		std::vector<PlatformDevice*>& platform_devices = m_ihl->get_platform_devices();
		std::size_t size = platform_devices.size();

		for (unsigned int i = 0; i < size; i++)
		{
			PlatformDevice* platform_device = platform_devices[i];
			Device& device = devices[i];

			//! Update device state
			platform_device->update_state();

			//! Detecting which of the active contexts needs to be updated
			for (const auto& context : m_context_stack)
			{
				//! Only consider contexts interested in current device
				if (!context->has_device(device))
					continue;

				//! Device callback
				if (platform_device->has_changed())
				{
					std::vector<Control*>& changed_controls = platform_device->get_updated_controls();

					context->call_callback(device, changed_controls);
				}
			}
		}
	
	}
}