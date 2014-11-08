//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_window_internal.hpp"

// C++ STD
#include <string>
#include <functional>
#include <vector>

namespace ssa
{
	enum class KeyCode : unsigned int
	{
		LeftArrow = 0x25,
		UpArrow = 0x26,
		RightArrow = 0x27,
		DownArrow = 0x28,
	};

	ssa_export class Window
	{
		friend LRESULT CALLBACK default_callback_proc(HWND, UINT, WPARAM, LPARAM);
	public:
		typedef window_handle_t handle_t;

		enum class Style
		{
			Default,
			Popup
		};

		enum class Orientation
		{
			Portrait,
			Landscape
		};

		enum class Event
		{
			Resize,
			KeyUp,
			KeyDown
		};

		typedef std::function<void(Event, Window&, unsigned int, unsigned int)> callback_t;

	public:
		Window();
		~Window();

		virtual bool create(unsigned int p_width, unsigned int p_height, const std::string& p_caption, Style p_style = Style::Default, Orientation p_orientation = Orientation::Landscape, bool p_fullscreen = false);
		virtual bool set_style(Style p_style);
		virtual bool set_orientation(Orientation);
		virtual bool set_size(unsigned int p_width, unsigned int p_height);
		virtual bool set_fullscreen(bool p_value);
		virtual bool set_caption(const std::string& p_caption);
		virtual bool set_cursor_visible(bool p_value);
		virtual bool move(unsigned p_x, unsigned int p_y);

		virtual void update();

		const handle_t& get_handle()const { return m_window_handle; }
		unsigned int get_width()const { return m_width; }
		unsigned int get_height()const { return m_height; }
		bool is_fullscreen()const { return m_is_fullscreen; }
		Style get_style()const { return m_style; }
		Orientation get_orientation()const { return m_orientation; }

		void register_callback(Event p_event, const callback_t& p_callback);

	protected:
		callback_t m_resize_callback;
		callback_t m_keyup_callback;
		callback_t m_keydown_callback;

		handle_t m_window_handle;
		WindowData m_window_data;

		unsigned int m_width;
		unsigned int m_height;
		bool		 m_is_fullscreen;
		std::string	 m_caption;
		Style		 m_style;
		Orientation  m_orientation;
	};
}