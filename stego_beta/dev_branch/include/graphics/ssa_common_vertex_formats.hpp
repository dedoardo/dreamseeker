//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "../core/ssa_math.hpp"

namespace ssa
{
	struct ssa_export VertexPC
	{
		float3 position;
		float4 color;
	};

	struct ssa_export VertexPT
	{
		float3 position;
		float2 texture;
	};

	struct ssa_export VertexPCT
	{
		float3 position;
		float4 color;
		float texture[2];
	};
}