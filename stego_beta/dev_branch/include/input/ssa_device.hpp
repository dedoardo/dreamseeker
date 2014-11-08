//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_types.hpp"
#include "ssa_control.hpp"

// C++ STD
#include <vector>
#include <functional>
#include <unordered_map>

namespace ssa
{
	class Device
	{
	public:
		enum class Type : U8
		{
			Pointer = 0x01,
			Keyboard = 0x02,
			Joystick = 0x04,
			Custom = 0x08 //! < Not supported yet
		};

	public :
		struct Data
		{
			U64				id;
			std::wstring	name;
			std::wstring	product_name;
			Type			type;
		};

		struct Capabilities
		{
			size_t buttons; //! Number of buttons, that can be either pressed or unpressed 
			size_t axis;	//! Number of axis, they include analogs and triggers
			size_t dpads;	//! Number of directional pads
		};
		
		
	public :
		Device(Data p_data, Capabilities p_capabilities);

		~Device() = default;

		//! Device is acquired by the application for use and is configured to pool data
		//virtual bool configure(/*Window& _window*/ HWND window_handle, bool _always_get_data = true);

		const Data&						get_data()const { return m_data; }

		const Capabilities&				get_capabilities()const { return m_capabilities; }

		std::vector<Control>*			get_controls() { return m_controls; }

		void							set_controls(std::vector<Control>& p_controls) { m_controls = &p_controls; }
	private :
		

		bool							m_acquired;

		Data							m_data;
		Capabilities					m_capabilities;
		std::vector<Control>*			m_controls;
	};
}