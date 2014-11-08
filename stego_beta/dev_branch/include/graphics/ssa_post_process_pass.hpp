//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// Includes
#include "ssa_shader.hpp"

// ssa
#include "../core/ssa_platform.hpp"

namespace ssa
{
	// Forward declarations
	class RenderDevice;

	//! \brief Class that represents a single post-process pass ( full-screen render + pixel shader ) 
	class ssa_export PostProcessPass
	{
	public :
		PostProcessPass(RenderDevice& p_render_device) :
			m_render_device{ p_render_device },
			m_shader{ p_render_device } { } 

		Shader& get_shader() { return m_shader; }
		virtual void set_params() = 0;

	protected :
		RenderDevice&	m_render_device;

		Shader			m_shader;
	};
}