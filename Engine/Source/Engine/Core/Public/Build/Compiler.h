/*
Ludo Game Engine
Copyright (C) 2016 TwinDrills

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

/// \brief: TODO
#if !defined(LD_SHIPPING_BUILD) && !defined(LD_RELEASE_BUILD) && !defined(LD_DEBUG_BUILD)
#error "No compile configuration defined. One of the following should always be defined: LD_SHIPPING_BUILD, LD_RELEASE_BUILD, LD_DEBUG_BUILD"
#endif

/// \brief: TODO
#if !defined(LD_PLATFORM_WINDOWS) && !defined(LD_PLATFORM_LINUX) && !defined(LD_PLATFORM_MACOS)
#error "No compile platform defined. One of the following should always be defined: LD_PLATFORM_WINDOWS, LD_PLATFORM_LINUX, LD_PLATFORM_MACOS"
#endif

/// \brief: TODO
#if !defined(LD_ARCHITECTURE_X86) && !defined(LD_ARCHITECTURE_X64)
#error "No compile architecture defined. One of the following should always be defined: LD_ARCHITECTURE_X86, LD_ARCHITECTURE_X64"
#endif

/// \brief: TODO
#define UNUSED_PARAMETER(x) ((void)(x))

/// \brief: TODO
#define ARRAY_LENGTH(x) (sizeof((x)) / sizeof(*(x)))

/// \brief: TODO
typedef signed char			int8;
typedef short				int16;
typedef int					int32;
typedef long long			int64;
typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int		uint32;
typedef unsigned long long	uint64;

#if defined(LD_ARCHITECTURE_X86)
typedef int32				intptr;
#else
typedef int64				intptr;
#endif
