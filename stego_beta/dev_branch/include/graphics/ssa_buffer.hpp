//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa 
#include "ssa_resource.hpp"
#include "ssa_commander.hpp"
#include "ssa_render_device.hpp"

namespace ssa
{
	class ssa_export Buffer : public Resource
	{
		friend class RenderDevice;
	
	// Just renaming some enumerators
	public:
		typedef BufferType Type;

	public:
		Buffer(RenderDevice& p_render_device) :
			Resource{ m_render_device } { } 
		virtual ~Buffer() = default;

		ssa_force_inline bool create(BufferType p_type, std::size_t p_element_size, std::size_t p_element_count, bool p_dynamic, const void* p_data)
		{
			return m_render_device.create_buffer(p_type, p_element_size, p_element_count, p_dynamic, p_data, *this);
		}

		ssa_force_inline bool update(Buffer& p_buffer, UpdateType p_type, const void* p_data, std::size_t p_size)
		{
			return m_render_device.update_buffer(*this, p_type, p_data, p_size);
		}

		ssa_force_inline bool bind()const
		{
			return m_render_device.bind_buffer(*this);
		}

		ssa_force_inline void destroy()
		{
			m_render_device.destroy_buffer(*this);
		}

		//! \brief Returns the type of the buffer
		//! \return Type of the buffer
		Type get_type()const { return m_type; }

		//! \brief Returns the size of a single element in the buffer
		//! \return Size of a single element in the buffer ( usually called stride )
		std::size_t get_stride()const { return m_stride; }

	protected :
		Type		m_type;
		std::size_t m_stride;
	};
}