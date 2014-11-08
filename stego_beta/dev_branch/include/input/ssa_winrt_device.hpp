//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_platform_device.hpp"
#include "ssa_control.hpp"

// C++ STD
#include <vector>

namespace ssa
{
	class WinRTDevice : public PlatformDevice
	{
	public:

		WinRTDevice();

		struct State
		{
			U64 x_axis{ 0 };
			U64 y_axis{ 0 };
			bool is_touch_pressed{ false };

			bool operator!=(State& other)
			{
				return (this->x_axis != other.x_axis || 
					this->y_axis != other.y_axis ||
					this->is_touch_pressed != other.is_touch_pressed);
			}
		};

		State& get_system_state();
		
		void update_state();

		bool has_changed();

		std::vector<Control>&			get_controls() { return m_controls; };

		std::vector<Control*>&			get_updated_controls();


	private:
		State							m_current_state;
		State							m_previous_state;
		State							m_system_state;

		std::vector<Control>			m_controls;

		std::vector<Control*>			m_changed_controls;
	};
}