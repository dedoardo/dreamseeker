//////////! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <input/ssa_device.hpp>s

// C++ STD
#include <cassert>

namespace ssa
{
	Device::Device(Data p_data, Capabilities p_capabilities) :
		m_acquired{ false },
		m_data( p_data ),
		m_capabilities( p_capabilities )
	{
	}
}