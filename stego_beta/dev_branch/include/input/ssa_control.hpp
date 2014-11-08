//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_types.hpp"

// C++ STD
#include <string>
#include <wrl.h>
#include <utility>

namespace ssa
{
	struct Control
	{
		enum class Type
		{
			Button,
			Axis,
			DPad,
		};

		struct
		{
			Type					type;
			std::wstring			name;
			I64						delta{ 0 };
			I64						state{ 0 };
			std::pair<I64, I64>		state_bounds;
		};
		
	};
}