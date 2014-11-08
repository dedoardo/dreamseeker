//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_resource.hpp"

namespace ssa
{
	class ssa_export Sampler : public Resource
	{
		friend class RenderDevice;

	public :
		typedef SamplerFilterType Filter;

	public :
		Sampler(RenderDevice& p_render_device) :
			Resource{ p_render_device } { } 
		virtual ~Sampler() = default;

		ssa_force_inline bool create(SamplerFilterType p_filter)
		{
			return m_render_device.create_sampler(p_filter, *this);
		}

		ssa_force_inline void destroy()
		{
			m_render_device.destroy_sampler(*this);
		}

		Filter get_filter()const { return m_filter; }

	protected :
		Filter m_filter;
	};
}