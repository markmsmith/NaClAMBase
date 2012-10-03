/*
  Copyright (c) 2010 John McCutchan <john@johnmccutchan.com>

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
  claim that you wrote the original software. If you use this software
  in a product, an acknowledgment in the product documentation would be
  appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and must not be
  misrepresented as being the original software.

  3. This notice may not be removed or altered from any source
  distribution.
*/

/* This include file will set defines indicating the PLATFORM, CPU, ARCH, COMPILER and BUILD type:
 *
 * PAL_PLATFORM_WINDOWS
 * PAL_PLATFORM_APPLE
 * PAL_PLATFORM_ANDROID
 * PAL_PLATFORM_PS3
 * PAL_PLATFORM_LINUX
 * PAL_PLATFORM_NACL
 *
 * PAL_CPU_X86
 * PAL_CPU_PPC
 * PAL_CPU_ARM
 * PAL_CPU_SPU
 *
 * PAL_ARCH_32BIT
 * PAL_ARCH_64BIT
 *
 * PAL_ENDIAN_LITTLE
 * PAL_ENDIAN_BIG
 *
 * PAL_COMPILER_MICROSOFT
 * PAL_COMPILER_SN
 * PAL_COMPILER_GNU
 *
 * PAL_BUILD_DEBUG
 * PAL_BUILD_RELEASE
 *
 * PAL_GRAPHICS_SDL
 * PAL_GRAPHICS_OPENGL
 * 
 * PAL_INLINE
 * PAL_NO_INLINE
 *
 * PAL_LIBRARY_PRESENT
 */

#if defined(__LITTLE_ENDIAN__)
#define PAL_ENDIAN_LITTLE
#else
#define PAL_ENDIAN_BIG
#endif

#if defined(__APPLE__)
#define PAL_PLATFORM_APPLE
#define PAL_CPU_X86
#define PAL_INLINE inline __attribute__((always_inline))
#define PAL_NO_INLINE __attribute__((noinline))
#if defined(__x86_64)
#define PAL_ARCH_64BIT
#else
#define PAL_ARCH_32BIT
#endif

#endif

#if defined(__native_client__)
#define PAL_PLATFORM_NACL
#define PAL_COMPILER_GNU
#define PAL_CPU_X86
#define PAL_ARCH_32BIT
#define PAL_ENDIAN_LITTLE
#define PAL_INLINE inline __attribute__((always_inline))
#define PAL_NO_INLINE __attribute__((noinline))
#endif

#if defined(__GNUC__)
#define PAL_COMPILER_GNU
#endif

#if defined(SN_TARGET_PS3)
#define PAL_PLATFORM_PS3
#define PAL_CPU_PPC
#define PAL_ARCH_32BIT
#define PAL_COMPILER_SN
#define PAL_INLINE __attribute__((always_inline))
#define PAL_NO_INLINE __attribute__((noinline))
#endif

#if !defined(PAL_PLATFORM_NACL) && defined(_WIN32) && defined(_MSC_VER) && defined(_M_IX86)
#define PAL_PLATFORM_WINDOWS
#define PAL_ARCH_32BIT
#define PAL_CPU_X86
// this is needed so that when <windows.h> is included, it doesn't include winsock.h
#define _WINSOCKAPI_
#elif !defined(PAL_PLATFORM_NACL) && defined(_WIN32) && defined(_MSC_VER) && defined(_M_X64)
#define PAL_PLATFORM_WINDOWS
#define PAL_ARCH_64BIT
#define PAL_CPU_X86
#define _WINSOCKAPI_
#endif

#if defined(_MSC_VER)
#define PAL_COMPILER_MICROSOFT
#define PAL_INLINE __inline
#define PAL_NO_INLINE __declspec(noinline)
#define PAL_TLS __declspec(thread)
#endif

#if defined(PAL_COMPILER_MICROSOFT)

#if defined(_DEBUG)
#define PAL_BUILD_DEBUG
#elif defined(NDEBUG)
#define PAL_BUILD_RELEASE
#endif

#endif // PAL_COMPILER_MICROSOFT

#if defined(PAL_COMPILER_SN)

#if defined(_DEBUG)
#define PAL_BUILD_DEBUG
#else
#define PAL_BUILD_RELEASE
#endif

#endif // PAL_COMPILER_SN

#if defined(PAL_PLATFORM_APPLE)

#if defined(DEBUG)
#define PAL_BUILD_DEBUG
#else
#define PAL_BUILD_RELEASE
#endif

#endif

#ifndef NULL
#define NULL 0
#endif

#define PAL_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&); \
  void operator=(const TypeName&)

#define PAL_LIBRARY_PRESENT 1

void palBreakHere();

