//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// C++ STD
#include <vector>

namespace ssa
{
	typedef std::vector<unsigned char> loaded_data;
	
	bool load_png(const std::string& p_filename, loaded_data& p_data, unsigned long& p_width, unsigned long& p_height);
}