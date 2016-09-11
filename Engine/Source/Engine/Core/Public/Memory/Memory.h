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

/// \brief TODO
enum class StandardMemoryArena
{
#define DEFINE_MEMORY_ARENA(Name, BasebResizable, BaseDefaultSize, LocationHint, DisplayName, DisplayDescription) Name,
#include "Core/Private/Memory/MemoryArenas.inc"
#undef DEFINE_MEMORY_ARENA

	Count
};

/// \brief TODO
enum class MemoryLocationHint
{
	CPU,
	GPU
};

/// \brief TODO
const int DefaultAlignment = -1;

/// \brief TODO
void GetPlatformArenaStateOverride(StandardMemoryArena Arena, bool& bResizable, int& InitialSize);

/// \brief TODO
void* Allocate(int Size, MemoryLocationHint LocationHint, int Alignment = DefaultAlignment);

/// \brief TODO
void* Reallocate(void* Ptr, int Size);

/// \brief TODO
void Free(void* Ptr);

}; // namespace Memory
}; // namespace Ludo

