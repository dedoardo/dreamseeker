//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

// Header
#include <core/ssa_entry_point.hpp>

#if defined(ssa_os_windows)

#include <shellapi.h>

int main(int argc, char* argv[])
{
	cmd_args_t cmd_args;

	// Format arguments
	for (int i{ 0 }; i < argc; ++i)
		cmd_args.push_back(argv[i]);

	return entry_point(cmd_args, ssa::Platform::Win32);
}

int WINAPI WinMain(HINSTANCE p_instance, HINSTANCE p_prev_instance, LPSTR p_cmd_line, int p_cmd_show)
{
	cmd_args_t cmd_args;

	std::string cmd_line{ p_cmd_line };
	std::wstring wcmd_line;
	wcmd_line.assign(cmd_line.begin(), cmd_line.end());

	int num_args{ 0 };
	auto args = CommandLineToArgvW(wcmd_line.c_str(), &num_args);

	// Format arguments
	for (int i{ 0 }; i < num_args; ++i)
	{
		std::wstring warg{ args[0] };
		std::string arg;
		arg.assign(warg.begin(), warg.end());
	}

	// Initializing hardware layer

	auto result = entry_point(cmd_args, ssa::Platform::Win32);
	LocalFree(args);

	return result;
};

#elif defined(ssa_os_windows_phone)

#elif defined(ssa_os_linux)

#elif defined(ssa_os_macos)

#endif