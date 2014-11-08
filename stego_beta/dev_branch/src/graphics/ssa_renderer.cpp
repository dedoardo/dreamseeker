//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <graphics/ssa_renderer.hpp>

// ssa
#include <graphics/ssa_texture.hpp>

namespace ssa
{
	bool			Renderer::s_locked{ false };
	Renderer const* Renderer::s_locker{ nullptr };

	Renderer::Renderer(RenderDevice& p_render_device, const Renderer& p_self) :
		m_render_device{ p_render_device },
		m_self{ &p_self },

		m_offscreen_rt{ p_render_device }
	{
		if (!m_render_device.create_texture(1920, 1080, Format::RGBA32Float, false, nullptr, m_offscreen_rt))
			std::abort();
	}

	Renderer::~Renderer()
	{

	}

	///////////////////////////////////////////////////////////////////////
	/// LOCKING
	///////////////////////////////////////////////////////////////////////

	bool Renderer::can_begin()
	{
		if (s_locked && (s_locker != m_self))
			return false;

		s_locked = true;
		s_locker = m_self;
		return true;
	}

	bool Renderer::can_render()
	{
		if (!s_locked || (s_locker != m_self))
			return false;

		return true;
	}

	bool Renderer::can_end()
	{
		if (!s_locked ||
			s_locker != m_self)
			return false;

		s_locked = false;
		s_locker = nullptr;

		return true;
	}

	///////////////////////////////////////////////////////////////////////
	/// POST PROCESSING
	///////////////////////////////////////////////////////////////////////
	void Renderer::push_effect(PostProcessEffect& p_effect)
	{
		m_pp_effects.push_back(p_effect);
	}

	void Renderer::pop_effect()
	{
		//@ TODO: Fix all this assert() thing
		assert(!m_pp_effects.empty());

		if (!m_pp_effects.empty())
			m_pp_effects.pop_back();
	}

	void Renderer::_post_process(Texture& p_render_target)
	{
		// @TODO : Add support for multiple effects

		for (auto& effect : m_pp_effects)
		{
			effect.get().process(p_render_target);
		}
	}
}