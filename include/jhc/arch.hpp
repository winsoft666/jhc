/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2022 JiangXueqiao <winsoft666@outlook.com>.
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef JHC_ARCH_HPP__
#define JHC_ARCH_HPP__

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define JHC_WIN 1
#ifdef _WIN64
#define JHC_WIN64 1
#else
#define JHC_WIN32 1
#endif
#elif defined(__APPLE__)
#define JHC_MACOS 1
#include <TargetConditionals.h>
#if defined(TARGET_IPHONE_SIMULATOR)
#elif defined(TARGET_OS_IPHONE)
#elif defined(TARGET_OS_MAC)
#else
#error "Unknown Apple platform"
#endif
#elif defined(__linux__) || defined(linux) || defined(__linux)
#define JHC_LINUX 1
#elif defined(__unix__) || defined(unix) || defined(__unix)
#define JHC_UNIX 1
#elif defined(_POSIX_VERSION)
#define JHC_POSIX 1
#else
#error "Unknown compiler"
#endif

// Processor architecture detection.
// For more info on what's defined, see:
//   http://msdn.microsoft.com/en-us/library/b0084kay.aspx
//   http://www.agner.org/optimize/calling_conventions.pdf
//   or with gcc, run: "echo | gcc -E -dM -"
#if defined(_M_X64) || defined(__x86_64__)
#define JHC_ARCH_X86_FAMILY 1
#define JHC_ARCH_X86_64 1
#define JHC_ARCH_64_BITS 1
#define JHC_ARCH_LITTLE_ENDIAN 1
#elif defined(_M_ARM64) || defined(__aarch64__)
#define JHC_ARCH_ARM_FAMILY 1
#define JHC_ARCH_64_BITS 1
#define JHC_ARCH_LITTLE_ENDIAN 1
#elif defined(_M_IX86) || defined(__i386__)
#define JHC_ARCH_X86_FAMILY 1
#define JHC_ARCH_X86 1
#define JHC_ARCH_32_BITS 1
#define JHC_ARCH_LITTLE_ENDIAN 1
#elif defined(__ARMEL__)
#define JHC_ARCH_ARM_FAMILY 1
#define JHC_ARCH_32_BITS 1
#define JHC_ARCH_LITTLE_ENDIAN 1
#elif defined(__MIPSEL__)
#define JHC_ARCH_MIPS_FAMILY 1
#if defined(__LP64__)
#define JHC_ARCH_64_BITS 1
#else
#define JHC_ARCH_32_BITS 1
#endif
#define JHC_ARCH_LITTLE_ENDIAN 1
#elif defined(__pnacl__)
#define JHC_ARCH_32_BITS 1
#define JHC_ARCH_LITTLE_ENDIAN 1
#elif defined(__EMSCRIPTEN__)
#define JHC_ARCH_32_BITS 1
#define JHC_ARCH_LITTLE_ENDIAN 1
#else
#error Please add support for your architecture in jhc/arch.hpp
#endif

#if !(defined(JHC_ARCH_LITTLE_ENDIAN) ^ defined(JHC_ARCH_BIG_ENDIAN))
#error Define either JHC_ARCH_LITTLE_ENDIAN or JHC_ARCH_BIG_ENDIAN
#endif

#endif  // !JHC_ARCH_HPP__