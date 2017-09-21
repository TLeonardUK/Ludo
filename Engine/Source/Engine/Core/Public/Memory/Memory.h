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

#include "Core/Public/Memory/MemoryArena.h"

namespace Ludo {
namespace Memory  {

/// \brief Identifes a list of standard memory arenas which are initialized on engine start.
enum class StandardMemoryArena
{
#define DEFINE_MEMORY_ARENA(Name, BasebResizable, BaseDefaultSize, LocationHint, DisplayName, DisplayDescription) Name,
#include "Core/Private/Memory/MemoryArenas.inc"
#undef DEFINE_MEMORY_ARENA

	Count
};

/* \brief Used to hint to the platform implementation what memory we want allocations to be stored in.
 *
 * This is not a guarantee of where the memory will end up being located, as different platforms may not have
 * specialized memory for different tasks.
 */
enum class MemoryLocationHint
{
	/** \brief Primary CPU accessible memory bank.
	 *
         * Typically slow to read from GPU but fast from CPU. Used for storing data-structures and memory
	 * required for game logic.
	 */
	CPU, 
	
	/** \brief Primary GPU accessible memory bank.
	 *
         * Typically slow to read from CPU but fast from GPU. Used for storing textures, vertex/index buffers,
	 * and other things the gpu will typically access.
	 */
	GPU 
};

/* \brief Default alignment all memory pointers are aligned to. 
 *
 * If number is negative a suitable alignment will be selected by the platform implementation.
 */
const int DefaultAlignment = -1;
 
/** \brief Overridden by platform implementation. Allows platform to adjust the properties
 *         of the standard memory areans allocated on startup.
 * 
 *  \param Arena	Arena in question.
 *  \param bResizable	Determines if the arena can resize itself when it becomes full, or
 *                      if it should fail to allocate when full.
 *  \param InitialSize  Size in bytes that the arena should be initialized at.
 */
void GetPlatformArenaStateOverride(StandardMemoryArena Arena, bool& bResizable, int& InitialSize);

/** \brief Overridden by platform implementation. Allocates a block of memory from the host
 *         platform in the given hardware location.
 * 
 *  \param Size         Size in bytes of memory to allocate.
 *  \param LocationHint Hint as to hardware location memory should be stored in.
 *  \param Alignment    Alignment allocated memory pointer should be aligned to. If negative
 *                      a suitable alignment will be chosen to suit the platform.
 *
 *  \returns		Pointer to allocated memory, or nullptr on failure.
 */
void* Allocate(int Size, MemoryLocationHint LocationHint, int Alignment = DefaultAlignment);

/** \brief Overridden by platform implementation. Reallocates a block of memory previously 
 *         allocated by Allocate.
 * 
 *  \param Ptr  Pointer to memory block we want to reallocate.
 *  \param Size New size of memory block we want to resize to.
 *
 *  \returns Pointer to reallocated memory block, may be the same as pointer passed in. Will
 *           return nullptr on failure.
 */
void* Reallocate(void* Ptr, int Size);

/** \brief Overridden by platform implementation. Frees a block of memory previously 
 *         allocated by Allocate.
 * 
 *  \param Ptr Pointer to block of memory to free.
 */
void Free(void* Ptr);

}; // namespace Memory
}; // namespace Ludo

