//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// ssa
#include <input/ssa_context.hpp>

namespace ssa
{
	void Context::add_device(Device& _device, const device_callback_t& _callback)
	{
		bool found{ false };
		for (const auto& callback : m_callbacks)
		{
			if (callback.device == &_device)
				found = true;
		}

		if (!found)
			m_callbacks.push_back({ &_device, _callback });
	}

	bool Context::has_device(Device& p_device)
	{
		for (const auto& callback : m_callbacks)
		{
			Device* a = &p_device;
			if (callback.device == a)
				return true;
		}

		return false;
	}

	void Context::remove_device(Device& _device)
	{
		auto& iter = m_callbacks.begin();
		for (auto& iter = m_callbacks.begin(); iter != m_callbacks.end();)
		{
			if (iter->device == &_device)
				iter = m_callbacks.erase(iter);
			else
				++iter;
		}
	}

	void Context::call_callback(Device& p_device, const std::vector<Control*>& p_controls)
	{
		for (auto& callback : m_callbacks)
		{
			if (callback.device == &p_device)
				callback.device_callback(p_device, p_controls);
		}
	}
}