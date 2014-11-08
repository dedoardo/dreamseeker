//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_platform_device.hpp"
#include "ssa_types.hpp"
#include "ssa_control.hpp"
#include <window\ssa_window.hpp>

// DInput
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")

// Win32
#include <wrl.h>
#include <cassert>

// C++ STD
#include <vector>

namespace ssa
{
	class WindowsDevice : public PlatformDevice
	{
	public:

		WindowsDevice();
		~WindowsDevice() = default;

		U64								m_id;

		//! Represents the current state of the device
		struct State
		{
			std::vector<DWORD>			controls_state;
		};

		void update_state();

		bool has_changed();

		std::vector<Control*>&			get_updated_controls();

		bool configure(Window& p_window);

		Microsoft::WRL::ComPtr<IDirectInputDevice8>	m_device;


		State&							get_current_state()const { return *m_current_state; }

		State&							get_previous_state()const { return *m_previous_state; }

		std::vector<Control>&			get_controls() { return m_controls; };

	private:
		State							m_state_buffer_0;
		State							m_state_buffer_1;

		State*							m_current_state;
		State*							m_previous_state;

		std::vector<Control>			m_controls;

		std::vector<Control*>			m_changed_controls;

		bool _first_pull{ true };

		bool _configure_controls(Window& p_window);

		bool _init_state();
	};
}