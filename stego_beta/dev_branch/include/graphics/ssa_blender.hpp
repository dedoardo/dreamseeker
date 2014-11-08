//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_resource.hpp"	
#include "ssa_commander_params.hpp"
#include "ssa_render_device.hpp"

namespace ssa
{
	class ssa_export Blender : public Resource
	{
		friend class RenderDevice;
	public :
		typedef BlendType Type;
	
	public : 
		Blender(RenderDevice& p_render_device) :
			Resource{ p_render_device } { }
		virtual ~Blender() = default;

		ssa_force_inline bool create(BlendType p_type) { return m_render_device.create_blender(p_type, *this); }
		ssa_force_inline bool bind()const { return m_render_device.bind_blender(*this); }
		ssa_force_inline void destroy() { return m_render_device.destroy_blender(*this); }
		

	protected :
		Type m_type;
	};
}