//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// C++ STD
#include <cstdlib>
#include <cstdint>
#include <list>
#include <cassert>

// ssa
#include "../core/ssa_platform.hpp"

namespace ssa
{

	class ssa_export Bag
	{
	public:
		typedef std::uint64_t index_t;

	public:
		//! Constructs a new instance of Bag and allocates the initial buffer
		//! \param p_element_size
		//! \param p_initial_buffer_size
		Bag(std::size_t p_element_size, std::size_t p_initial_buffer_size);

		//! Deallocates all internal memory
		~Bag();

		template <typename object_t, typename ...ctor_args>
		index_t add_object(ctor_args ...p_args);

		template <typename object_t>
		object_t& get_object(index_t p_index);

		void recycle(index_t p_index);

		uint8_t* get_data_ptr()					{ return m_data; }
		std::size_t get_element_size()const		{ return m_element_size; }
		index_t get_last_element_pos()const 
		{ 
			return m_current_size; 
		}

	private:
		void _safe_release();

		// Resizes array if necessary
		index_t _get_next_spot();

	private:
		// List of free spots in the buffer
		std::list<index_t>	m_free_list;

		// Actual buffer
		uint8_t*				m_data;

		// Size of the single item in the buffer
		std::size_t			m_element_size;
		index_t				m_current_size;
	};

	// === TEMPLATE ===
	template <typename object_t, typename ...ctor_args>
	Bag::index_t Bag::add_object(ctor_args ...p_args)
	{
		assert(m_data != nullptr);

		// Taking the first free spot
		index_t next_spot = _get_next_spot();

		// Constructing object & copying it
		//object_t object{p_args...};
		*reinterpret_cast<object_t*>(m_data + next_spot * m_element_size) = object_t(p_args...);
		//std::memcpy(m_data + next_spot * m_element_size,
		//	reinterpret_cast<void*>(&object),
		//	sizeof(object));

		return next_spot;
	}

	template <typename object_t>
	object_t& Bag::get_object(Bag::index_t p_index)
	{
		assert(m_data != nullptr);

		return *reinterpret_cast<object_t*>(m_data + p_index * m_element_size);
	}
}