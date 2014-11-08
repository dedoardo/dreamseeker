//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// Stegosaurus version
#define ssa_version_major 0
#define ssa_version_minor 1

// Compiler macos
#if defined(_MSC_VER)
#define ssa_compiler_msvc
#elif defined(__GNUC__)
#define ssa_compiler_gcc
#elif defined(__clang__)
#define ssa_compiler_clang
#else
#error The compiler you are currently building with is not supported, see requirements.txt for more informations
#endif

#if !defined(__cplusplus)
#error C++11 compiler needed, see requirements.txt for more informations
#endif 

// Platform macros
// Reserved for Windows 8 + // @TODO : 
#include <winapifamily.h>

#if WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
#include <windows.h>
#define ssa_os_windows
#elif WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
#define ssa_os_windows_phone
#elif defined(__linux__)
#define ssa_os_linux
#error All linux-based operating systems are not supported yet
#elif defined(macintosh) || defined(Macintosh) || (defined(__APPLE__) && defined(__MACH__))
#define ssa_os_macos
#error All MacOses are not supported yet
#else
#error Operating system not recognized and probably not supported, see requirements.txt for more informations
#endif

// Architecture ( more to add ), we don't care  about the compiler here
#if defined(ssa_os_windows) && defined(_WIN64)
#define ssa_arch_64
#elif defined(__amd64__) || defined(_M_X64)
#define ssa_arch_64
#else 
#define ssa_ARCH_32
#endif

// Language specific 
#if defined(ssa_compiler_msvc)
#define ssa_force_inline __forceinline
#elif defined(ssa_compiler_gcc)
#define ssa_force_inline __attribute__((always_inline))
#elif defined(ssa_compiler_clang)
#define ssa_force_inline
#else
#define ssa_force_inline
#endif

// Import export macros
// Windows Phone builds only as a DLL
#ifdef ssa_os_windows_phone
#	ifdef ssa_compiler_msvc
#		define ssa_export __declspec(dllexport)
#	elif defined(ssa_compiler_gcc)
#		error Not supported yet
#	elif defined(ssa_compiler_clang)
#		error Not supported yet
#	endif
#endif

#ifndef ssa_export
#	ifdef ssa_static_build
#		ifdef ssa_compiler_msvc
#			define ssa_export __declspec(dllexport)
#		elif defined(ssa_compiler_gcc)
#			define ssa_export
#		elif defined(ssa_compiler_clang)
#			#define ssa_export
#		endif
#	else
#		define ssa_export 
#	endif
#endif

namespace ssa
{
	enum class Platform
	{
		Win32, // Desktop
		WinRT  // Mobile
	};
}

#define ssa_constexpr 
