//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include <graphics/ssa_buffer.hpp>
#include <graphics/ssa_shader.hpp>

namespace ssa
{
	// Forward declaration
	class RenderDevice;

	// Uses VertexPT inside
	class FullscreenQuad
	{
	public :
		FullscreenQuad(RenderDevice& p_render_device);

		void render();

	protected :
		RenderDevice& m_render_device;
		Buffer m_buffer;
		Shader m_vertex_shader;
	};
}