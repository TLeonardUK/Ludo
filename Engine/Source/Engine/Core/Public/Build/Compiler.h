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

// Check for valid configuration defines.
#if !defined(LD_SHIPPING_BUILD) && !defined(LD_RELEASE_BUILD) && !defined(LD_DEBUG_BUILD)
#error "No compile configuration defined. One of the following should always be defined: LD_SHIPPING_BUILD, LD_RELEASE_BUILD, LD_DEBUG_BUILD"
#endif

// Check for valid platform defines.
#if !defined(LD_PLATFORM_WINDOWS) && !defined(LD_PLATFORM_LINUX) && !defined(LD_PLATFORM_MACOS)
#error "No compile platform defined. One of the following should always be defined: LD_PLATFORM_WINDOWS, LD_PLATFORM_LINUX, LD_PLATFORM_MACOS"
#endif

// Check for valid archiecture defines.
#if !defined(LD_ARCHITECTURE_X86) && !defined(LD_ARCHITECTURE_X64)
#error "No compile architecture defined. One of the following should always be defined: LD_ARCHITECTURE_X86, LD_ARCHITECTURE_X64"
#endif

/** \brief Supresses warnings on unused parameters.
 *
 *  \tparam ParameterName Pararmeter to supress warnings for.
 */
#define LD_UNUSED_PARAMETER(ParameterName) ((void)(ParameterName))

/** \brief Calculates the length of an statically declared array. 
 *          This will not work on dynamic memory allocations.
 *
 *  \tparam ParameterName Pararmeter to supress warnings for.
 *  \returns Size in bytes of array.
 */
#define LD_ARRAY_LENGTH(Array) (sizeof((Array)) / sizeof(*(Array)))

/// \brief Signed integer 8 bits in length.
typedef signed char			int8;

/// \brief Signed integer 16 bits in length.
typedef signed short		int16;

/// \brief Signed integer 32 bits in length.
typedef signed int  		int32;

/// \brief Signed integer 64 bits in length.
typedef signed long long	int64;

/// \brief Unsigned integer 8 bits in length.
typedef unsigned char		uint8;

/// \brief Unsigned integer 16 bits in length.
typedef unsigned short		uint16;

/// \brief Unsigned integer 32 bits in length.
typedef unsigned int		uint32;

/// \brief Unsigned integer 64 bits in length.
typedef unsigned long long	uint64;

/// \brief Type long enough to store a pointer.
#if defined(LD_ARCHITECTURE_X64)
typedef uint64	        	intptr;
#else
typedef uint32		        intptr;
#endif

