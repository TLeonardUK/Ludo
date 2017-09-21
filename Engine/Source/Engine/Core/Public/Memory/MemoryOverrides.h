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

// This file contains all the default memory configurations.

#include "Core/Public/Memory/MemoryArena.h"
#include "Core/Public/Memory/Memory.h"

namespace Ludo {

/** \brief Defines an standard arena class that code can allocate memory from.
 *
 *  \param Name			Name of this arena. Final class will be called NameMemoryArena.
 *  \param BasebResizable	Default resizable state. If true the arena can grow when it runs out of space, 
 *				otherwise it will fail when it runs out of memory. Can be overriden by platform
 *				implementation.
 *  \param BaseDefaultSize	Default size of the memory the arena is allocated on start. Can be overridden by 
 *				platform implementation.
 *  \param LocationHint		Location where backing memory for this arena should be located. This acts as a hint
 *				to the platform implementation as to where to physically store the memoy.
 *  \param DisplayName		Display name of this arena as shown in human-readable messages.
 *  \param DisplayDescription	Description of this arena as shown in human-readable messages.
 */
#define DEFINE_MEMORY_ARENA(Name, BasebResizable, BaseDefaultSize, LocationHint, DisplayName, DisplayDescription) \
	class Name##MemoryArena : public IMemoryArena \
	{ \
	public: \
		const char* GetName() const override \
		{ \
			return DisplayName; \
		} \
		const char* GetDescription() const override \
		{ \
			return DisplayDescription; \
		} \
		void GetDefaultState(bool& bResizable, int& DefaultSize) const override \
		{ \
			bResizable = BasebResizable; \
			DefaultSize = BaseDefaultSize; \
			Memory::GetPlatformArenaStateOverride(Memory::StandardMemoryArena::Name, bResizable, DefaultSize); \
		} \
		void* Allocate(const int Count) override \
		{ \
			return Memory::Allocate(Count, Memory::MemoryLocationHint::LocationHint); \
		} \
		void* Reallocate(void* Ptr, const int Count) override \
		{ \
			return Memory::Reallocate(Ptr, Count); \
		} \
		void Free(void* Ptr) override \
		{ \
			return Memory::Free(Ptr); \
		} \
	}; 

#include "Core/Private/Memory/MemoryArenas.inc"

#undef DEFINE_MEMORY_ARENA

}; // nanmespace Ludo

