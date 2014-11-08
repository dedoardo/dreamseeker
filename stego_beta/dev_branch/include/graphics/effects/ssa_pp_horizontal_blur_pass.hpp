//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "../ssa_post_process_pass.hpp"

namespace ssa
{
	class HorizontalBlurPass : public PostProcessPass
	{
	public :
		HorizontalBlurPass(RenderDevice& p_render_device);

		void set_params()override;
	};
}