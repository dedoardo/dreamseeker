//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <input/ssa_winrt_device.hpp>

namespace ssa
{
	WinRTDevice::WinRTDevice()
	{
		Control x_axis_control;
		x_axis_control.name = L"Asse X";
		x_axis_control.type = Control::Type::Axis;

		Control y_axis_control;
		y_axis_control.name = L"Asse Y";
		y_axis_control.type = Control::Type::Axis;

		Control touch_control;
		touch_control.name = L"Touch";
		touch_control.type = Control::Type::Button;

		m_controls.push_back(x_axis_control);
		m_controls.push_back(y_axis_control);
		m_controls.push_back(touch_control);
	}

	WinRTDevice::State& WinRTDevice::get_system_state()
	{
		return m_system_state;
	}

	void WinRTDevice::update_state()
	{
		m_current_state = m_system_state;
	}

	bool WinRTDevice::has_changed()
	{
		return m_current_state != m_previous_state;
		
	}

	std::vector<Control*>& WinRTDevice::get_updated_controls()
	{
		m_changed_controls.clear();
		
		if (m_current_state != m_previous_state)
		{
			if (m_current_state.x_axis != m_previous_state.x_axis)
			{
				m_controls[0].delta = m_current_state.x_axis - m_previous_state.x_axis;
				m_changed_controls.push_back(&m_controls[0]);
			}

			if (m_current_state.y_axis != m_previous_state.y_axis)
			{
				m_controls[1].delta = m_current_state.y_axis - m_previous_state.y_axis;
				m_changed_controls.push_back(&m_controls[1]);
			}

			if (m_current_state.is_touch_pressed != m_previous_state.is_touch_pressed)
			{
				m_controls[2].delta = m_current_state.is_touch_pressed - m_previous_state.is_touch_pressed;
				m_changed_controls.push_back(&m_controls[2]);
			}
		}

		return m_changed_controls;
	}
}