//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <window/ssa_window.hpp>

// ssa
#include <core/ssa_platform.hpp>

// C++ STD
#include <cassert>
#include <unordered_map>

#ifdef ssa_os_windows

namespace ssa
{
	namespace
	{
		std::unordered_map<HWND , Window*> m_active_windows;
	}

	LRESULT CALLBACK default_callback_proc(HWND p_hwnd, UINT p_msg, WPARAM p_wparam, LPARAM p_lparam)
	{
		auto find_result = m_active_windows.find(p_hwnd);
		if (find_result == m_active_windows.end())
			return DefWindowProc(p_hwnd, p_msg, p_wparam, p_lparam);

		auto& window = *(find_result->second);

		// Window is registered
		switch (p_msg)
		{
		case WM_SIZE:
			// Updating window information
			window.m_width = LOWORD(p_lparam);
			window.m_height = HIWORD(p_lparam);
			if (find_result->second->m_resize_callback)
				find_result->second->m_resize_callback(Window::Event::Resize, window, 0, 0);
			break;
		case WM_KEYDOWN:
			if (find_result->second->m_keydown_callback)
				find_result->second->m_keydown_callback(Window::Event::KeyDown, window, p_wparam, 0);
			break;
		case WM_KEYUP:
			if (find_result->second->m_keyup_callback)
				find_result->second->m_keyup_callback(Window::Event::KeyUp, window, p_wparam, 0);
			break;
		}


		return DefWindowProc(p_hwnd, p_msg, p_wparam, p_lparam);
	}

	Window::Window() : 
		m_window_handle{ nullptr },
		m_width{ 0 }, 
		m_height{ 0 },
		m_is_fullscreen{ false },
		m_caption{},
		m_style{ Style::Default },
		m_orientation{ Orientation::Landscape }
	{
		m_window_data.class_created = false;
	}

	Window::~Window()
	{
		if (m_window_handle != nullptr)
		{
			CloseWindow(m_window_handle);
			m_active_windows.erase(m_active_windows.find(m_window_handle));
		}
	}

	bool Window::create(unsigned int p_width, unsigned int p_height, const std::string& p_caption, Style p_style, Orientation p_orientation, bool p_fullscreen)
	{
		static std::wstring s_class_name{ L"ssa_window" };

		if (!m_window_data.class_created)
		{
			// Creating the base class for all the windows that will be created
			WNDCLASSEX window_class_ex;
			window_class_ex.cbSize = sizeof(WNDCLASSEX);
			window_class_ex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			window_class_ex.lpfnWndProc = default_callback_proc;
			window_class_ex.cbClsExtra =
			window_class_ex.cbWndExtra = 0;
			window_class_ex.hInstance = GetModuleHandle(nullptr); // @TODO : Call Platform::get_app_instance()
			window_class_ex.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPLICATION));
			window_class_ex.hCursor = LoadCursor(NULL, IDC_ARROW);
			window_class_ex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			window_class_ex.lpszMenuName = NULL;
			window_class_ex.lpszClassName = s_class_name.c_str();
			window_class_ex.hIconSm = LoadIcon(window_class_ex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
		
			if (!RegisterClassEx(&window_class_ex))
				return false;

			m_window_data.class_created = true;
		}

		// Converting caption to wstring
		std::wstring wcaption;
		wcaption.assign(p_caption.begin(), p_caption.end());

		DWORD win32_style = WS_OVERLAPPEDWINDOW;
		if (p_style == Style::Popup)
			win32_style = WS_POPUP;

		m_window_handle = CreateWindow(
			s_class_name.c_str(),
			wcaption.c_str(),
			win32_style,
			CW_USEDEFAULT, CW_USEDEFAULT,
			p_width, p_height,
			nullptr, nullptr,
			GetModuleHandle(NULL),
			nullptr);

		if (!m_window_handle)
			return false;

		m_active_windows.insert(std::make_pair(m_window_handle, this));

		ShowWindow(m_window_handle, SW_SHOW);
		UpdateWindow(m_window_handle);

		// Updating information
		m_width = p_width;
		m_height = p_height;
		m_style = p_style;
		m_orientation = p_orientation;
		m_caption = p_caption;

		set_fullscreen(p_fullscreen);

		return true;
	}

	bool Window::set_style(Style p_style)
	{
		assert(m_window_handle != nullptr);

		// Changing style
		if (p_style == m_style)
			return true;

		DWORD win32_style = WS_OVERLAPPEDWINDOW;
		if (p_style == Style::Popup)
			win32_style = WS_POPUP;

		RECT window_rect;
		if (!GetWindowRect(m_window_handle, &window_rect))
			return false;

		if (!SetWindowLongPtr(m_window_handle, GWL_STYLE, win32_style))
			return false;

		if (!SetWindowPos(m_window_handle, HWND_TOPMOST, window_rect.left, window_rect.top,
			window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, SWP_SHOWWINDOW))
			return false;

		m_style = p_style;

		return true;
	}

	bool Window::set_orientation(Orientation p_orientation)
	{
		assert(m_window_handle != nullptr);

		return true;
	}

	bool Window::set_size(unsigned int p_width, unsigned int p_height)
	{
		assert(m_window_handle != nullptr);

		RECT window_rect;
		if (!GetWindowRect(m_window_handle, &window_rect))
			return false;

		if (!SetWindowPos(m_window_handle, HWND_TOPMOST, window_rect.left, window_rect.top,
			p_width, p_height, SWP_SHOWWINDOW))
			return false;

		m_width = p_width;
		m_height = p_height;

		return true;
	}
	
	bool Window::set_fullscreen(bool p_value)
	{
		assert(m_window_handle != nullptr);

		if (m_is_fullscreen == p_value)
			return true;

		if (m_width == 0 || m_height == 0)
			return false;

		if (p_value)
		{
			unsigned int display_counter{ 0 };
			DISPLAY_DEVICE display_device;
			display_device.cb = sizeof(DISPLAY_DEVICE);
			while (!FAILED(EnumDisplayDevices(nullptr, display_counter++, &display_device, 0)))
			{
				// Using only devices attached to desktop
				if (!(display_device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP))
					continue;

				DEVMODE display_mode;
				unsigned int mode_counter{ 0 };
				display_mode.dmSize = sizeof(DEVMODE);
				while (!FAILED(EnumDisplaySettings(display_device.DeviceName, mode_counter++, &display_mode)))
				{
					if (m_width == display_mode.dmPelsWidth &&
						m_height == display_mode.dmPelsHeight)
					{
						// Setting display resolution
						ChangeDisplaySettings(&display_mode, CDS_FULLSCREEN);

						// Setting window style
						set_style(Style::Popup);
						move(0, 0);

						m_is_fullscreen = true;
						return true;
					}
				}
			}
		}
		else
		{
			// Resetting to default
			ChangeDisplaySettings(nullptr, 0);
			m_is_fullscreen = false;
			return true;
		}

		return false;
	}

	bool Window::set_caption(const std::string& p_caption)
	{
		assert(m_window_handle != nullptr);

		if (!SetWindowTextA(m_window_handle, p_caption.c_str()))
			return false;

		m_caption = p_caption;

		return true;
	}

	bool Window::set_cursor_visible(bool p_value)
	{
		assert(m_window_handle != nullptr);

		if (FAILED(ShowCursor(p_value)))
			return false;

		return true;
	}

	bool Window::move(unsigned int p_x, unsigned int p_y)
	{
		assert(m_window_handle != nullptr);

		if (!SetWindowPos(m_window_handle, HWND_TOPMOST, p_x, p_y, m_width, m_height, SWP_SHOWWINDOW))
			return false;
		
		return true;
	}

	void Window::update()
	{
		static MSG next_message;
		next_message.message = WM_NULL;

		if (PeekMessage(&next_message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&next_message);
			DispatchMessage(&next_message);
		}
	}

	void Window::register_callback(Event p_event, const callback_t& p_callback)
	{
		if (p_event == Event::Resize)
			m_resize_callback = p_callback;
		else if (p_event == Event::KeyDown)
			m_keydown_callback = p_callback;
		else if (p_event == Event::KeyUp)
			m_keyup_callback = p_callback;
	}
}

#endif