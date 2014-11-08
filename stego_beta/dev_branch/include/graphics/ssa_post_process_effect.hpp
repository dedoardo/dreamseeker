//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt
#pragma once

// C++ STD
#include <vector>
#include <utility>

// ssa
#include "../core/ssa_platform.hpp"
#include "ssa_post_process_pass.hpp"

namespace ssa
{
	// Forward declaration
	class Texture;
	class RenderDevice;

	//! \brief Collection of PostProcessPass
	class ssa_export PostProcessEffect
	{
	public :
		virtual void process(Texture& p_input_target) = 0;

		std::vector<std::reference_wrapper<PostProcessPass>> passes;
	};
}