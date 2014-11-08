//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "../core/ssa_platform.hpp"
#include "ssa_commander_internals.hpp"

namespace ssa
{
	inline ssa_constexpr DXGI_FORMAT format_to_raw(Format p_format)
	{
		switch (p_format)
		{
		case Format::RGBA8Unorm:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case Format::RGBA32Float:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		default:
			return DXGI_FORMAT_UNKNOWN;
		}

		DXGI_FORMAT_UNKNOWN;
	}

	inline ssa_constexpr Format raw_to_format(DXGI_FORMAT p_raw)
	{
		switch (p_raw)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return Format::RGBA8Unorm;
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			return Format::RGBA32Float;
		default :
			return Format::Unknown;
		}
	}

	inline ssa_constexpr bool is_render_target_format_valid(Format p_format)
	{
		return true;
	}

	inline ssa_constexpr D3D11_FILTER filter_to_raw(SamplerFilterType p_filter)
	{
		switch (p_filter)
		{
		case SamplerFilterType::Point:
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		case SamplerFilterType::Linear:
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		case SamplerFilterType::Anisotropic:
			return D3D11_FILTER_ANISOTROPIC;
		default :
			return D3D11_FILTER_ANISOTROPIC;
		}
	}

	inline ssa_constexpr D3D11_MAP map_to_raw(UpdateType p_update)
	{
		switch (p_update)
		{
		case UpdateType::Discard:
			return D3D11_MAP_WRITE_DISCARD;
		case UpdateType::NoOverwrite:
			return D3D11_MAP_WRITE_NO_OVERWRITE;
		default:
			return D3D11_MAP_WRITE_DISCARD;
		}
	}
}