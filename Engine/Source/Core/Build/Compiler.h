// Copyright 2016 TwinDrills
#pragma once

// This file contains general defines used to distinguish compiler and platform features.

/// \brief: TODO
#if !defined(LD_SHIPPING_BUILD) && !defined(LD_RELEASE_BUILD) && !defined(LD_DEBUG_BUILD)
#pragma warning("No compile configuration defined. One of the following should always be defined: LD_SHIPPING_BUILD, LD_RELEASE_BUILD, LD_DEBUG_BUILD")
#endif

/// \brief: TODO
#if !defined(LD_PLATFORM_WINDOWS)
#pragma warning("No compile platform defined. One of the following should always be defined: LD_PLATFORM_WINDOWS")
#endif

/// \brief: TODO
#if !defined(LD_ARCHITECTURE_X86) && !defined(LD_ARCHITECTURE_X64)
#pragma warning("No compile architecture defined. One of the following should always be defined: LD_ARCHITECTURE_X64, LD_ARCHITECTURE_X86")
#endif

/// \brief: TODO
#if !defined(LD_LITTLE_ENDIAN) && !defined(LD_BIG_ENDIAN)
#pragma warning("No compile endian defined. One of the following should always be defined: LD_LITTLE_ENDIAN, LD_BIG_ENDIAN")
#endif

/// \brief: TODO
#define UNUSED_PARAMETER(x) ((void)(x))

/// \brief: TODO
#define ARRAY_LENGTH(x) (sizeof((x)) / sizeof(*(x)))

// Disable secure function warnings, we won't be using them for portability reasons.
#if defined(_MSC_VER)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS (1)
#endif
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE (1)
#endif
#pragma warning(disable : 4996)
#pragma warning(disable : 4995)
#pragma warning(disable : 4722)
#endif
