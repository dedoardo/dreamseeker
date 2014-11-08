//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <input/ssa_windows_device.hpp>

#include <iostream>

namespace ssa
{
	WindowsDevice::WindowsDevice()
	{
	}

	// === INIT FUNCTIONS ===

	bool WindowsDevice::configure(Window& p_window)
	{
		//! Clearing previous containers
		m_state_buffer_0.controls_state.clear();
		m_state_buffer_1.controls_state.clear();
		m_controls.clear();
		m_changed_controls.clear();

		//! Preparing device
		HWND window_handle = p_window.get_handle();
		if (FAILED(m_device.Get()->SetCooperativeLevel(window_handle, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		{
			return false;
		}

		//! Configure device controls
		if (!_configure_controls(p_window))
			return false;


		//! Acquire device
		if (FAILED(m_device.Get()->Acquire()))
		{
			return false;
		}

		//! Perform first state pull and initialize state buffers
		if (!_init_state())
			return false;

		return true;
	}

	struct _control_creation_param
	{
		std::vector<DIOBJECTDATAFORMAT>*		formats;
		std::vector<Control>*					controls;
		Window*									window;
	};

	BOOL FAR PASCAL _create_controls_callback(LPCDIDEVICEOBJECTINSTANCEW _object_instance, LPVOID _data_ptr)
	{
		DIOBJECTDATAFORMAT object_format;
		ZeroMemory(&object_format, sizeof(DIOBJECTDATAFORMAT));

		std::wcout << _object_instance->tszName;
		std::cout << " : " <<  _object_instance->dwOfs << std::endl;

		GUID empty = { 0 };

		_control_creation_param* param = reinterpret_cast<_control_creation_param*>(_data_ptr);

		//object_format.pguid = _object_instance->guidType != empty? &_object_instance->guidType : nullptr;
		object_format.pguid = 0;
		object_format.dwType = DIDFT_ANYINSTANCE | _object_instance->dwType;
		object_format.dwOfs = sizeof(DWORD)* param->formats->size();
		object_format.dwFlags = 0;

		param->formats->push_back(object_format);

		param->controls->emplace_back();
		Control& new_control = param->controls->back();
		ZeroMemory(&new_control, sizeof(Control));

		if (_object_instance->dwType & DIDFT_AXIS)
		{
			new_control.type = Control::Type::Axis;
		}
		else if (_object_instance->dwType & DIDFT_BUTTON |
			_object_instance->dwType & DIDFT_PSHBUTTON |
			_object_instance->dwType & DIDFT_TGLBUTTON)
		{
			new_control.type = Control::Type::Button;
		}
		else if (_object_instance->dwType & DIDFT_POV)
		{
			new_control.type = Control::Type::DPad;
		}

		new_control.name = _object_instance->tszName;

		if (_object_instance->guidType == GUID_XAxis)
		{
			new_control.state = param->window->get_width() / 2.f;
			new_control.state_bounds.first = 0;
			new_control.state_bounds.second = param->window->get_width();
		}
		else if (_object_instance->guidType == GUID_YAxis){
			new_control.state = param->window->get_height() / 2.f;
			new_control.state_bounds.first = 0;
			new_control.state_bounds.second = param->window->get_height();
		}

		return DIENUM_CONTINUE;
	}

	bool WindowsDevice::_configure_controls(Window& p_window)
	{
		//! Prepare callback parameter
		std::vector<DIOBJECTDATAFORMAT> formats;
		
		_control_creation_param param;
		param.formats = &formats;
		param.controls = &m_controls;
		param.window = &p_window;

		//! First off we enumerate all the objects
		HRESULT hr = m_device.Get()->EnumObjects(
			_create_controls_callback,
			reinterpret_cast<void*>(&param),
			DIDFT_AXIS | DIDFT_BUTTON | DIDFT_PSHBUTTON | DIDFT_TGLBUTTON | DIDFT_POV);

		if (FAILED(hr))
		{
			// LOG(Error::Type::Warning, "Failed to enumerate device objects");
			return false;
		}

		//! Then we create the data_format structure and pass it to the DIDevice
		DIDATAFORMAT new_format;
		new_format.dwSize = sizeof(new_format);
		new_format.dwObjSize = sizeof(DIOBJECTDATAFORMAT);
		new_format.dwFlags = DIDF_ABSAXIS;

		new_format.dwNumObjs = formats.size();
		new_format.rgodf = &formats[0];
		new_format.dwDataSize = sizeof(DWORD)* formats.size();

		hr = m_device.Get()->SetDataFormat(&new_format);
		if (FAILED(hr))
		{
			// LOG(Error::Type::Warning, "Failed to set a new data format");
			return false;
		}

		//! Finally we prepare the data buffers
		m_state_buffer_0.controls_state.resize(formats.size());
		m_state_buffer_1.controls_state.resize(formats.size());

		m_current_state = &m_state_buffer_0;
		m_previous_state = &m_state_buffer_1;

		return true;
	}

	bool WindowsDevice::_init_state()
	{
		HRESULT hr = m_device->GetDeviceState(sizeof(DWORD)* m_controls.size(),
			reinterpret_cast<void*>(&m_previous_state->controls_state[0]));
		if (FAILED(hr))
		{
			// LOG(Error::Type::Warning, "Failed to retrieve the device state ");
			return false;
		}

		std::memcpy(reinterpret_cast<void*>(&m_current_state->controls_state[0]),
			reinterpret_cast<void*>(&m_previous_state->controls_state[0]),
			sizeof(m_current_state->controls_state[0]) * m_current_state->controls_state.size());
	}

	// ==== RUNTIME FUNCTIONS ===

	void WindowsDevice::update_state()
	{
		assert(m_device.Get() != nullptr);

		HRESULT hr = m_device->GetDeviceState(sizeof(DWORD)* m_controls.size(),
			reinterpret_cast<void*>(&m_previous_state->controls_state[0]));
		if (FAILED(hr))
		{
			// LOG(Error::Type::Warning, "Failed to retrieve the device state ");
		}

		State* t = m_previous_state;
		m_previous_state = m_current_state;
		m_current_state = t;
	}

	bool WindowsDevice::has_changed()
	{
		return std::memcmp(
				reinterpret_cast<void*>(&m_previous_state->controls_state[0]), 
				reinterpret_cast<void*>(&m_current_state->controls_state[0]),
				sizeof(m_current_state->controls_state[0]) * m_current_state->controls_state.size()
			) != 0;
	}

	std::vector<Control*>& WindowsDevice::get_updated_controls()
	{
		m_changed_controls.clear();
		unsigned int i{ 0 };

		for (auto& control : m_controls)
		{
			if (m_current_state->controls_state[i] != m_previous_state->controls_state[i])
			{

				I64 delta = (static_cast<std::int64_t>(m_current_state->controls_state[i]) - static_cast<std::int64_t>(m_previous_state->controls_state[i]));
				if (delta > 2294967296)
					delta -= 4294967296;
				else if (-delta > 2294967296)
					delta += 4294967296;


				if (control.type == Control::Type::Button)
				{
					delta /= 128;
					if (delta > 0)
						control.state = 1;
					else if (delta < 0)
						control.state = 0;
				}
				else if (control.type == Control::Type::Axis)
				{
					control.state += delta;		
					if (control.state < control.state_bounds.first)
						control.state = control.state_bounds.first;
					if (control.state > control.state_bounds.second)
						control.state = control.state_bounds.second;
				}

				control.delta = delta;

				m_changed_controls.push_back(&(m_controls[i]));
			}
			i++;
		}

		return m_changed_controls;
	}
}