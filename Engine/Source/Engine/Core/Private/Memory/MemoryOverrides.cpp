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

#include "PCH.h"

#include "Core/Public/Debug/Log.h"
#include "Core/Public/Memory/MemoryOverrides.h"
#include "Core/Public/Types/String.h"
#include "Core/Public/Memory/ArenaAllocator.h"

#define OPT_DUMP_LEGACY_NEW_WARNINGS 0

namespace Ludo
{
#if OPT_DUMP_LEGACY_NEW_WARNINGS
	static bool gLegacyMemoryReEntranceGuard = false;
#endif

	void* LegacyNew(std::size_t count)
	{
#if OPT_DUMP_LEGACY_NEW_WARNINGS
		if (!gLegacyMemoryReEntranceGuard)
		{
			gLegacyMemoryReEntranceGuard = true;
			LogF(LogMemory, LogWarning, "Allocated %i bytes through default new. Use LD_NEW instead.", count);
			gLegacyMemoryReEntranceGuard = false;
		}
#endif

		Ludo::ArenaAllocator<Ludo::LibCMemoryArena> allocator;
		return allocator.Alloc((int)count);
	}

	void* LegacyNewArray(std::size_t count)
	{
#if OPT_DUMP_LEGACY_NEW_WARNINGS
		if (!gLegacyMemoryReEntranceGuard)
		{
			gLegacyMemoryReEntranceGuard = true;
			LogF(LogMemory, LogWarning, "Allocated %i bytes through default new[]. Use LD_NEW_ARRAY instead.", count);
			gLegacyMemoryReEntranceGuard = false;
		}
#endif

		Ludo::ArenaAllocator<Ludo::LibCMemoryArena> allocator;
		return allocator.Alloc((int)count);
	}

	void LegacyDelete(void* ptr)
	{
#if OPT_DUMP_LEGACY_NEW_WARNINGS
		if (!gLegacyMemoryReEntranceGuard)
		{
			gLegacyMemoryReEntranceGuard = true;
			LogF(LogMemory, LogWarning, "Deleted memory through default delete. Use LD_DELETE instead.");
			gLegacyMemoryReEntranceGuard = false;
		}
#endif

		Ludo::ArenaAllocator<Ludo::LibCMemoryArena> allocator;
		allocator.Free(ptr);
	}

	void LegacyDeleteArray(void* ptr)
	{
#if OPT_DUMP_LEGACY_NEW_WARNINGS
		if (!gLegacyMemoryReEntranceGuard)
		{
			gLegacyMemoryReEntranceGuard = true;
			LogF(LogMemory, LogWarning, "Deleted memory through default delete[]. Use LD_DELETE_ARRAY instead.");
			gLegacyMemoryReEntranceGuard = false;
		}
#endif

		Ludo::ArenaAllocator<Ludo::LibCMemoryArena> allocator;
		allocator.Free(ptr);
	}
};

void* operator new(std::size_t count)
{
	return Ludo::LegacyNew(count);
}

void* operator new[](std::size_t count)
{
	return Ludo::LegacyNewArray(count);
}

void* operator new(std::size_t count, const std::nothrow_t& tag) throw()
{
	UNUSED_PARAMETER(tag);
	return Ludo::LegacyNew(count);
}

void* operator new[](std::size_t count, const std::nothrow_t& tag) throw()
{
	UNUSED_PARAMETER(tag);
	return Ludo::LegacyNewArray(count);
}

void operator delete(void* ptr) throw()
{
	Ludo::LegacyDelete(ptr);
}

void operator delete[](void* ptr) throw()
{
	Ludo::LegacyDeleteArray(ptr);
}

void operator delete(void* ptr, const std::nothrow_t& tag) throw()
{
	UNUSED_PARAMETER(tag);
	Ludo::LegacyDelete(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t& tag) throw()
{
	UNUSED_PARAMETER(tag);
	Ludo::LegacyDeleteArray(ptr);
}

