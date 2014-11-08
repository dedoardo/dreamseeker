//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_types.hpp"
#include "ssa_device.hpp"

// C++ STD
#include <string>
#include <unordered_map>
#include <list>
#include <functional>
#include <cstdint>

namespace ssa
{
	//! \brief A context defines the interaction with the input system
	//!
	//! A context is a set of callbacks and binds, it specifies which devices should 
	//! be taken in consideration and how to react to input events via callbacks
	class Context
	{
		friend class Dispatcher;
	public:
		typedef std::function<void(const Device&, const std::vector<Control*>&)> 
			device_callback_t;

	public:
		Context() = default;
		~Context() = default;

		//! Adds a device to the list of listening devices and links it to the given callback
		void add_device(Device& p_device, const device_callback_t& p_callback);
		
		//! Whether or not the context is listening to given device
		bool has_device(Device& p_device);

		//! Removes given device from the list of listening devices
		void remove_device(Device& p_device);

		//! Calls the callback linked to the given device, passing the given control vector as parameter
		void call_callback(Device& p_device, const std::vector<Control*>& p_controls);

	protected:
		struct DeviceCallback
		{
			Device* device;
			device_callback_t device_callback;
		};

		std::list<DeviceCallback> m_callbacks;
	};
}