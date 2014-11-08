//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_resource.hpp"
#include "ssa_commander.hpp"
#include "ssa_render_device.hpp"

// C++ STD
#include <string>

namespace ssa
{
	class ssa_export Shader : public Resource
	{
		friend class RenderDevice;
	public :
		typedef ShaderType Type;
		typedef shader_macro_t macro_t;

	public :
		Shader(RenderDevice& p_render_device) :
			Resource{ p_render_device } { } 
		virtual ~Shader() = default;

		ssa_force_inline bool create(ShaderType p_type, const std::string& p_code, const std::string& p_entry_point, const std::vector<std::pair<std::string, std::string>>& p_macros = shader_macro_t())
		{
			return m_render_device.create_shader(p_type, p_code, p_entry_point, p_macros, *this);
		}

		ssa_force_inline bool bind()
		{
			return m_render_device.bind_shader(*this);
		}

		ssa_force_inline void destroy()
		{
			m_render_device.destroy_shader(*this);
		}

		Type get_type()const { return m_type; };
		const std::string& get_code()const { return m_code; }

	protected :
		Type		m_type;
		std::string m_code;
	};
}