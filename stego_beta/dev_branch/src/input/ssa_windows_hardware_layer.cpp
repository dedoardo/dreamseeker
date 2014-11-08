//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <input/ssa_windows_hardware_layer.hpp>

// ssa
#include <input/ssa_platform_device.hpp>

namespace ssa
{
	WindowsHardwareLayer::WindowsHardwareLayer() :
		m_direct_input{ nullptr }
	{

	}

	WindowsHardwareLayer::~WindowsHardwareLayer()
	{
		if (m_direct_input != nullptr)
			m_direct_input->Release();
	}

	void WindowsHardwareLayer::init(Window& p_window)
	{
		if (m_direct_input != nullptr)
			m_direct_input->Release();

		m_direct_input = nullptr;

		HRESULT hr = DirectInput8Create(
			GetModuleHandle(nullptr),
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			reinterpret_cast<void**>(&m_direct_input),
			nullptr);

		if (FAILED(hr))
		{
			// LOG(Error::Type::Critical, "Failed to initialize DirectInput with error" + std::to_string(hr));
		}

		HWND window_handle = p_window.get_handle();
		create_devices(window_handle);

		configure_devices(p_window);
	}

	struct _device_creation_param
	{
		HWND*							window;
		IDirectInput8*					direct_input;
		std::vector<WindowsDevice>*		platform_devices;
		std::vector<Device>*			devices;
		Device::Type					type;
		U64*							next_id;
	};  

	BOOL FAR PASCAL _device_creation_callback(LPCDIDEVICEINSTANCEW _device_instance, LPVOID _data_ptr)
	{
		_device_creation_param* param = reinterpret_cast<_device_creation_param*>(_data_ptr);

		// == Platform device creation ==

		param->platform_devices->emplace_back();

		WindowsDevice& win_device = param->platform_devices->back();

		IDirectInputDevice8* di_device;
		HRESULT hr = param->direct_input->CreateDevice(
			_device_instance->guidInstance,
			&di_device,
			nullptr);

		if (FAILED(hr))
		{
			// LOG(Error::Type::Warning, "Failed to create the direct input device for the device named : " + utility::string::to_string(data.name));
			return DIENUM_CONTINUE;
		}

		win_device.m_id = *(param->next_id);
		win_device.m_device = di_device;
		//win_device.configure(*(param->window));

		// == ssa Device creation ==

		Device::Data data;

		data.id = *(param->next_id);
		data.name = _device_instance->tszInstanceName;
		data.product_name = _device_instance->tszProductName;
		data.type = param->type;


		Device::Capabilities capabilities;

		DIDEVCAPS DI_capabilities;
		ZeroMemory(&DI_capabilities, sizeof(DIDEVCAPS));
		DI_capabilities.dwSize = sizeof(DI_capabilities);

		hr = di_device->GetCapabilities(&DI_capabilities);
		if (FAILED(hr))
		{
		// LOG(Error::Type::Warning, "Failed to retrieve the device capabilities for " + utility::string::to_string(m_data.name) + "with error " + std::to_string(hr));
		}

		capabilities.buttons = DI_capabilities.dwButtons;
		capabilities.axis = DI_capabilities.dwAxes;
		capabilities.dpads = DI_capabilities.dwPOVs;

		param->devices->emplace_back(data, capabilities);

		(*(param->next_id))++;

		return DIENUM_CONTINUE;
	}

	void WindowsHardwareLayer::create_devices(HWND& p_window)
	{
		/*for (auto& device : m_devices)
		{
			memory::deallocate(device);
		}*/
		m_platform_devices.clear();
		m_devices.clear();

		_device_creation_param param;
		U64 next_id{ 0 };
		param.window = &p_window;
		param.direct_input = m_direct_input;
		param.devices = &m_devices;
		param.platform_devices = &_windows_devices;
		param.next_id = &(next_id);

		param.type = Device::Type::Joystick;

		m_direct_input->EnumDevices(DI8DEVCLASS_GAMECTRL,
			_device_creation_callback,
			reinterpret_cast<void*>(&param),
			DIEDFL_ATTACHEDONLY);

		param.type = Device::Type::Keyboard;

		m_direct_input->EnumDevices(DI8DEVCLASS_KEYBOARD,
			_device_creation_callback,
			reinterpret_cast<void*>(&param),
			DIEDFL_ATTACHEDONLY);

		param.type = Device::Type::Pointer;

		m_direct_input->EnumDevices(DI8DEVCLASS_POINTER,
			_device_creation_callback,
			reinterpret_cast<void*>(&param),
			DIEDFL_ATTACHEDONLY);
	}

	void WindowsHardwareLayer::configure_devices(Window& p_window)
	{
		for (unsigned int i = 0; i < _windows_devices.size(); i++)
		{
			_windows_devices[i].configure(p_window);

			m_devices[i].set_controls(_windows_devices[i].get_controls());

			m_platform_devices.push_back(&_windows_devices[i]);
		}
	}

	void WindowsHardwareLayer::update_devices()
	{
		for (auto& platform_device : m_platform_devices)
		{
			platform_device->update_state();
		}
	}
}