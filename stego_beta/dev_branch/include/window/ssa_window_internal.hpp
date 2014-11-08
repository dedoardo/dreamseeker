//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "../core/ssa_platform.hpp"

namespace ssa
{
#if defined(ssa_os_windows)
	typedef HWND window_handle_t;

	struct WindowData
	{
		WindowData() : class_created{ false } { } 

		bool class_created;
	};

#elif defined(ssa_os_windows_phone)

#include "../core/ssa_platform_layer.hpp"

	struct WindowData
	{
		
	};

	typedef Windows::UI::Core::CoreWindow^ window_handle_t;

#endif
}