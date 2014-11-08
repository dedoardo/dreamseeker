//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// C++ STD
#include <string>
#include <limits>

// ssa
#include "../core/ssa_platform.hpp"

namespace ssa
{
	//! \brief Base class for every resource handlers, it is simply a container
	//!		for an id or a name
	class ssa_export Resource
	{
		friend class RenderDevice;
	public :
		static const std::size_t invalid_id{ UINT_MAX };

	public:
		Resource(RenderDevice& p_render_device) :
			m_render_device{ p_render_device },
			m_id{ invalid_id } {}
		virtual ~Resource() = default;

		//! \brief Returns the id that identifies the resource
		//! \return ID of the resource
		std::size_t get_id()const { return m_id; }

		bool is_valid()const { return m_id != invalid_id; }

	protected:
		RenderDevice& m_render_device;
		std::size_t m_id;
	};
}