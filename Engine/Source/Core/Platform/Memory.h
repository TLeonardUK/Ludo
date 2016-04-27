// Copyright 2016 TwinDrills
#pragma once

namespace Ludo {
namespace Memory  {

/// \brief TODO
enum class StandardMemoryArena
{
	LibC,
	Containers,
	Strings,
	Platform
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
void GetArenaDefaultState(StandardMemoryArena Arena, bool& bResizable, int& InitialSize);

/// \brief TODO
void* Allocate(int Size, MemoryLocationHint LocationHint, int Alignment = DefaultAlignment);

/// \brief TODO
void* Reallocate(void* Ptr, int Size);

/// \brief TODO
void Free(void* Ptr);

}; // namespace Memory
}; // namespace Ludo