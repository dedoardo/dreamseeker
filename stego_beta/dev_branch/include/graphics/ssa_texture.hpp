//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_resource.hpp"
#include "ssa_commander_internals.hpp"
#include "ssa_render_device.hpp"
#include "../window/ssa_window.hpp"

namespace ssa
{
	class ssa_export Texture : public Resource
	{
		friend class RenderDevice;
	public:
		typedef Format Format;

	public:
		Texture(RenderDevice& p_render_device) : 
			Resource{ p_render_device } { }

		ssa_force_inline bool create(unsigned int p_width, unsigned int p_height, Format p_format, bool p_dynamic, const void* p_data)
		{
			return m_render_device.create_texture(p_width, p_height, p_format, p_dynamic, p_data, *this);
		}

		ssa_force_inline bool create_from_window(Window& p_window)
		{
			return m_render_device.create_render_window(p_window, *this);
		}

		ssa_force_inline void destroy()
		{
			return m_render_device.destroy_texture(*this);
		}

		const TextureData& get_data()const { return m_data; }

	protected:
		TextureData m_data;
	};
}