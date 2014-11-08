//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "../../core/ssa_math.hpp"

namespace ssa
{
	//! \brief Class that wraps all supported transformations in 2D space ( more to add ) 
	struct Transformable2D
	{
		Transformable2D() :
		position{ 0 },
		scale{ 1 },
		origin{ 0 },
		rotation{ 0 } { } 

		//! \brief Position defined in pixel coordinates, X-axis goes from left to right ( positive ) and from top to bottom ( positive ) 
		//!		origin is situated in the top-left corner of the window
		float2 position;

		//! \brief Scale of the object along the X and Y axis, defaults to 1.f for no scaling
		float2 scale;

		//! \brief Origin for transformation, before object is transformed it displaced by its origin, transformed and then moved back
		//!		particularly useful for rotations. It's defined in coordinates relative from the position of the object
		float2 origin;

		//! \brief Rotation along the Z-axis
		float  rotation;
	};
}