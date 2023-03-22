// Copyright (c) 1993-2022 Robert McNeel & Associates. All rights reserved.
// OpenNURBS, Rhinoceros, and Rhino3D are registered trademarks of Robert
// McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//
// For complete openNURBS copyright information see <http://www.opennurbs.org>.
//
////////////////////////////////////////////////////////////////

#if !defined(OPENNURBS_SYSTEM_RUNTIME_INC_)
#define OPENNURBS_SYSTEM_RUNTIME_INC_

////////////////////////////////////////////////////////////////
//
// Determines the runtime environment where the code is executed.
//
////////////////////////////////////////////////////////////////


/*
////////////////////////////////////////////////////////////
//
// BEGIN - MYON_RUNTIME_APPLE / MYON_RUNTIME_WIN / MYON_RUNTIME_ANDROID defines
//
// MYON_RUNTIME_* specifies the runtime C/C++ SDK being used
//   At most one the MYON_RUNTIME_* should be defined
//
//   MYON_RUNTIME_APPLE / MYON_RUNTIME_WIN / MYON_RUNTIME_ANDROID
//
*/
#if (defined(__APPLE__) || defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR) || defined(__IOS__))

#if !defined(MYON_RUNTIME_APPLE)
#define MYON_RUNTIME_APPLE
#endif

#elif defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(WINDOWS) || defined(_WINDOWS_) || defined(__WINDOWS__)

#if !defined(MYON_RUNTIME_WIN)
#define MYON_RUNTIME_WIN
#endif

#elif defined(__ANDROID__) || defined(__EMSCRIPTEN__) || defined(ANDROID)
// __EMSCRIPTEN__ is for a web assembly compile which currently compiles with the
// same settings as an android build. We will need to add an MYON_RUNTIME_WASM once
// the __EMSCRIPTEN__ compile stabilizes
#if !defined(MYON_RUNTIME_ANDROID)
#define MYON_RUNTIME_ANDROID
#endif

#elif defined(__linux__)
#if !defined(MYON_RUNTIME_LINUX)
#define MYON_RUNTIME_LINUX
#endif

#endif
/*
//
// END - MYON_RUNTIME_APPLE / MYON_RUNTIME_WIN / MYON_RUNTIME_ANDROID defines
//
////////////////////////////////////////////////////////////
*/

/*
////////////////////////////////////////////////////////////
//
// BEGIN - Additional platform defines
//
// MYON_64BIT_RUNTIME / MYON_32BIT_RUNTIME
// MYON_LITTLE_ENDIAN / MYON_BIG_ENDIAN
// MYON_SIZEOF_WCHAR_T
// MYON_RUNTIME_<PLATFORM>_<SUBPLATFORM>
//
*/
#if defined(MYON_RUNTIME_APPLE)
#include <TargetConditionals.h>

#if (TARGET_OS_IPHONE == 1)
#define MYON_RUNTIME_APPLE_IOS
#endif

#if (TARGET_OS_SIMULATOR == 1)
#define MYON_RUNTIME_APPLE_IOS
#define MYON_RUNTIME_APPLE_IOS_SIMULATOR
#endif

#if !defined(MYON_RUNTIME_APPLE_IOS)
#define MYON_RUNTIME_APPLE_MACOS

#if !defined(RHINO_CORE_COMPONENT)
// Apple:
//   Defines RHINO_CORE_COMPONENT here.
//   If we publish an Apple C++ pubic SDK, this will need to be adjusted.
// Windows:
//   uses the property sheet RhinoProjectPropertySheets/Rhino.Cpp.common.props
//   Some build products in Windows are not "core components"
#define RHINO_CORE_COMPONENT 1
#endif
#endif

#if (defined(__LP64__) || defined(__ppc64__))
#define MYON_64BIT_RUNTIME
#elif defined(__LP32__)
#define MYON_32BIT_RUNTIME
#else
#error Add code to detect sizeof pointer on this Apple platform
#endif

#define MYON_SIZEOF_WCHAR_T 4

#if (defined(__ppc__) || defined(__ppc64__))
#define MYON_BIG_ENDIAN
#else
#define MYON_LITTLE_ENDIAN
#endif

#elif defined(MYON_RUNTIME_WIN)

#define MYON_SIZEOF_WCHAR_T 2

#if defined(WINDOWS_PHONE)
#define MYON_RUNTIME_WIN_MOBILE
#else
#define MYON_RUNTIME_WIN_WINOS
#endif

#if defined(_M_X64) || defined(_WIN64)
#define MYON_64BIT_RUNTIME
#elif defined(_M_X86) || defined(_WIN32)
#define MYON_32BIT_RUNTIME
#else
#error Add code to detect sizeof pointer on this Windows platform
#endif

#if !defined(MYON_LITTLE_ENDIAN)
#if (defined(_M_X64) || defined(_M_IX86) || defined (__i386__) || defined( __x86_64__ ))
#define MYON_LITTLE_ENDIAN
#endif
#endif

#elif defined(MYON_RUNTIME_ANDROID)
// Android is Linux
#define MYON_RUNTIME_LINUX

#if defined(__x86_64__) || defined(__LP64__) || defined(__ppc64__)
#define MYON_64BIT_RUNTIME
#else
#define MYON_32BIT_RUNTIME
#endif

#if !defined(MYON_SIZEOF_WCHAR_T)
#define MYON_SIZEOF_WCHAR_T 4
#endif

// It looks like all android compiles are little endian.
// If there is a better test, please add it here
#if !defined(MYON_LITTLE_ENDIAN)
#define MYON_LITTLE_ENDIAN
#endif

#elif defined(MYON_RUNTIME_LINUX)

#if !defined(MYON_SIZEOF_WCHAR_T)
#define MYON_SIZEOF_WCHAR_T 4
#endif

#include <cstdint>
#if INTPTR_MAX == INT64_MAX
#define MYON_64BIT_RUNTIME
#elif INTPTR_MAX == INT32_MAX
#define MYON_32BIT_RUNTIME
#endif

#include <endian.h>
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define MYON_LITTLE_ENDIAN
#else
#define MYON_BIG_ENDIAN
#endif

#endif

#if !defined(MYON_64BIT_RUNTIME) && !defined(MYON_32BIT_RUNTIME)
/* Attempt to determing runtime pointer size */
#if (defined(_M_X64) || defined(__LP64__) || defined(__ppc64__))
#define MYON_64BIT_RUNTIME
#elif (defined(_M_X86) || defined(__LP32__))
#define MYON_32BIT_RUNTIME
#endif
#endif

#if defined(MYON_64BIT_RUNTIME) && defined(MYON_32BIT_RUNTIME)
#error Exactly one of MYON_64BIT_RUNTIME or MYON_32BIT_RUNTIME must be defined.
#endif

#if !defined(MYON_64BIT_RUNTIME) && !defined(MYON_32BIT_RUNTIME)
#error Exactly one of MYON_64BIT_RUNTIME or MYON_32BIT_RUNTIME must be defined.
#endif

#if defined(MYON_BIG_ENDIAN) && defined(MYON_LITTLE_ENDIAN)
#error Exactly one of MYON_LITTLE_ENDIAN or MYON_BIG_ENDIAN should be defined.
#endif

#if !defined(MYON_BIG_ENDIAN) && !defined(MYON_LITTLE_ENDIAN)
#error Exactly one of MYON_LITTLE_ENDIAN or MYON_BIG_ENDIAN should be defined.
#endif

#if (!defined(MYON_ARM_64) && defined(__aarch64__))
#define MYON_ARM_64
#endif

/*
//
// END - Additional platform defines
//
////////////////////////////////////////////////////////////
*/

#endif
