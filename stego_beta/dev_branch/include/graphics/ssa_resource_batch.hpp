//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// C++ STD
#include <algorithm>
#include <vector>
#include <list>

// ssa
#include "../core/ssa_platform.hpp"

namespace ssa
{
	template <typename resource_t>
	class ssa_export ResourceBatch
	{
	public:
		typedef std::size_t index_t;
	public:
		ResourceBatch(std::size_t p_initial_size);
		~ResourceBatch();

		index_t create_resource();

		resource_t& get(index_t p_index);

		void recycle(index_t p_index);

	private:
		std::vector<resource_t>						m_resources;
		std::list<index_t>							m_free_list;
	};

	template <typename resource_t>
	ResourceBatch<resource_t>::ResourceBatch(std::size_t p_initial_size)
	{
		m_resources.resize(p_initial_size);

		m_free_list.resize(m_resources.size());
		unsigned int counter{ 0 };
		std::generate(m_free_list.begin(), m_free_list.end(), [&] { return counter++; });
	}

	template <typename resource_t>
	ResourceBatch<resource_t>::~ResourceBatch()
	{
		// @TODO : implement deallocation callback
	}

	template <typename resource_t>
	typename ResourceBatch<resource_t>::index_t ResourceBatch<resource_t>::create_resource()
	{
		index_t next_free = m_free_list.front();
		m_free_list.pop_front();

		return next_free;
	}

	template <typename resource_t>
	resource_t& ResourceBatch<resource_t>::get(typename ResourceBatch<resource_t>::index_t p_index)
	{
		return m_resources[p_index];
	}

	template <typename resource_t>
	void ResourceBatch<resource_t>::recycle(typename ResourceBatch<resource_t>::index_t p_index)
	{
		// @TODO : implement deallocation callback
		m_free_list.push_front(p_index);
	}
}