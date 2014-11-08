//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_renderable2d.hpp"

namespace ssa
{
	//! \brief Also known as PointSprite it's a simple textured-quad and since it's the
	//!		most common shape indeed, is a semplification of a Renderable2D
	class Sprite : public Renderable2D
	{
	public :
		//! \brief Creates a new sprite of size (1, 1)
		Sprite();

		//! \brief Sets a new texture and new dimensions for the sprite
		//! 
		//! This call does an additional operation by resizing the sprite to the texture's dimensions
		//! \param [in] p_texture New texture
		void set_texture(const Texture& p_texture);
		void set_texture_rect(unsigned int p_x, unsigned int p_y, unsigned int p_width, unsigned int p_height);

		void set_rect(unsigned int p_width, unsigned int p_height);
	};
}