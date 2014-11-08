//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <core/ssa_bag.hpp>

// C++ STD
#include <algorithm>

namespace ssa
{
	Bag::Bag(std::size_t p_element_size, std::size_t p_initial_buffer_size) :
		m_data{ nullptr },
		m_element_size{ p_element_size },
		m_current_size{ 0 }
	{
		std::size_t to_allocate = m_element_size * p_initial_buffer_size;

		m_data = new uint8_t[to_allocate];

		m_current_size = p_initial_buffer_size;

		// Generating free spots
		m_free_list.resize(static_cast<std::size_t>(m_current_size));
		index_t index{ 0 };
		std::generate(m_free_list.begin(), m_free_list.end(), [&] { return index++; });
		std::memset(m_data, 0, m_element_size * static_cast<std::size_t>(m_current_size));
	}

	Bag::~Bag()
	{
		_safe_release();
	}

	void Bag::recycle(index_t p_index)
	{
		// Erasing object
		std::memset(m_data + p_index * m_element_size, 0, m_element_size);

		// Adding to free list
		m_free_list.push_front(p_index);
	}

	void Bag::_safe_release()
	{
		if (m_data != nullptr)
			delete[] m_data;
	}

	Bag::index_t Bag::_get_next_spot()
	{
		index_t next_spot{ m_free_list.front() };
		m_free_list.pop_front();
		return next_spot;
	}
}