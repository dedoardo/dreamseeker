//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// \brief Interface for the underlying Math API ( is cross-platform, we are just renaming for convenience )

// ssa
#include "ssa_platform.hpp"

// GLM
#include <glm/glm.hpp>

// C++ STD
#include <utility>

// @TODO : probably bugged, no compiler errors, but error when calling it with valid arguments
#define RENAME_FUNCTION(old_name, new_name) \
	template <typename ...args> \
	ssa_force_inline auto new_name(args&& ...p_args) -> decltype(old_name(std::forward<args>(p_args)...)) \
	{ \
	return old_name(std::forward<args>(p_args)...); \
	}

namespace ssa
{
	typedef glm::vec2 float2;
	typedef glm::vec3 float3;
	typedef glm::vec4 float4;

	typedef glm::ivec2 int2;
	typedef glm::ivec3 int3;
	typedef glm::ivec4 int4;

	typedef glm::f32mat2x2 float2x2;
	typedef glm::f32mat3x3 float3x3;
	typedef glm::f32mat4x4 float4x4; 

	//RENAME_FUNCTION(glm::sin<float>, sin);
	//RENAME_FUNCTION(glm::cos<float>, cos);
	//RENAME_FUNCTION(glm::degrees<float>, degrees);
	//RENAME_FUNCTION(glm::radians<float>, radians);

	// As far as it seems is quite redundant, but still : 
	// http://stackoverflow.com/questions/14391272/does-constexpr-imply-inline
	// constexpr functions imply inline, but it might be that constexpr is not supported
	// then is not redundant at all ( looking at you MSVC )

	inline ssa_export ssa_constexpr float sin(float p_angle)
	{
		return glm::sin(p_angle);
	}

	inline ssa_export ssa_constexpr float cos(float p_angle)
	{
		return glm::cos(p_angle);
	}

	inline ssa_export ssa_constexpr float degrees(float p_angle)
	{
		return glm::degrees(p_angle);
	}

	inline ssa_export ssa_constexpr float radians(float p_angle)
	{
		return glm::radians(p_angle);
	}
}