//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_render_device.hpp"
#include "ssa_post_process_effect.hpp"
#include "ssa_fullscreen_quad.hpp"
#include "ssa_sampler.hpp"
#include "ssa_texture.hpp"
#include "ssa_blender.hpp"

// C++ STD
#include <vector>
#include <utility>

namespace ssa
{
	//! \brief Base class for every renderer, it simplifies syncronization 
	//!		and helps not overlapping state setting and not screwing up rendering
	//!
	//! > What is the idea of a renderer ?
	//! 
	//! > How does locking works ? 
	class ssa_export Renderer
	{
	public:
		//! \brief Constructs a new Renderer instance
		//! 
		//! \param [in] p_render_device RenderDevice that will be used by the renderer for rendering, this *HAS* to be initialized
		//!		via init() before passing it, it might cause crashes otherwise
		//! \param [in] p_self This one is odd, this should be *this a reference to itself, this is needed
		//!		for registration / locking purposes
		Renderer(RenderDevice& p_render_device, const Renderer& p_self);

		//! \brief Default destructor, does not destroy associated resources
		~Renderer();

		//! \brief Returns the RenderDevice used to initialize the Renderer
		//! \return Reference to the RenderDevice used to initialize this Renderer
		RenderDevice& get_render_device()const { return m_render_device; }

		///////////////////////////////////////////////////////////////////////
		/// LOCKING
		///////////////////////////////////////////////////////////////////////

		//! \brief Checks if the renderer can begin a rendering session and not allow other
		//!		Renderers to render. It also locks the session itself
		//! \return True if session can begin and it locks it, false otherwise
		bool can_begin();

		//! \brief Checks if session has been locked by himself
		//! \return True if session belongs to him, false otherwise
		bool can_render();

		//! \brief Checks if session has been locked by himself, and if so, it releases it
		//!		leaving it open for other Rendereres to start rendering
		//! \return True if session has been locked by him and unlocks it, false otherwise
		bool can_end();

		///////////////////////////////////////////////////////////////////////
		/// POST PROCESSING
		///////////////////////////////////////////////////////////////////////
		void push_effect(PostProcessEffect& p_effect);

		void pop_effect();

	protected :
		RenderDevice& m_render_device;
	
		// Do postprocessing
		void _post_process(Texture& p_render_target);

	private :
		static bool				s_locked;
		static Renderer const*	s_locker;

		Renderer const* m_self;

	protected :
		Texture	m_offscreen_rt;
		std::vector<std::reference_wrapper<PostProcessEffect>> m_pp_effects;
	};
}