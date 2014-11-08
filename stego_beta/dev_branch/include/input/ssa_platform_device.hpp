//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_control.hpp"

// C++ STD
#include <vector>

namespace ssa
{
	class PlatformDevice
	{
	public:
		virtual void update_state() = 0;

		virtual bool has_changed() = 0;

		virtual std::vector<Control*>& get_updated_controls() = 0;
	};
}