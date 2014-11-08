//////! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_device.hpp"
#include "ssa_windows_device.hpp"
#include "ssa_hardware_layer.hpp"
#include "ssa_windows_hardware_layer.hpp"

// C++ STD
#include <list>

namespace ssa
{
	//! Forward declarations
	class Context;
	class HardwareLayer;

	//! \brief Class responsible of managing contexts and communicating to the HardwareLayer
	class Dispatcher
	{
	public:
		Dispatcher(HardwareLayer* p_hwl);
		~Dispatcher();

		//! Adds a context to the top of the context stack, this has the lowest priority
		void push_context(Context& _context);

		//! Removes the context at the top of the stack
		void pop_context();

		//! This is a sort of helper function 
		//! that invokes the HardwareLayer refresh() + config()
		//bool init(/*Window& _window*/ HWND window_handle);

		//! \brief Retrieves the current number of attached devices with the specified type
		
		//size_t enum_of_type(Device::Type _type);

		//Device* get_device(Device::Type _type, size_t _index = 0);

		//! Gathers the input, passes through the context stack and calls all the callbacks
		void dispatch();

		HardwareLayer& get_ihl()const { return *m_ihl; }

	private:
		std::list<Context*> m_context_stack;
		HardwareLayer*  m_ihl;
	};
}