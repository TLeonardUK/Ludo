// Copyright 2016 TwinDrills

#include "Core/Build/Memory.h"
#include "Core/Types/String.h"
#include "Core/Memory/ArenaAllocator.h"

/*
void* operator new(std::size_t count)
{
	
	Ludo::StringArgumentList Args;
	Args.Add("Bytes", (int)count);

	Log(LogMemory, LogWarning, "Allocated {Bytes} bytes through default new. Use placement new with a specific allocator instead.", Args);

	Ludo::ArenaAllocator<Ludo::LibCMemoryArena> allocator;
	return allocator.Alloc(count);

	return malloc(count);
}

void* operator new[](std::size_t count)
{
	
	Ludo::ArenaAllocator<Ludo::LibCMemoryArena> allocator;
	return allocator.Alloc(count);
	

	return malloc(count);
}

void* operator new(std::size_t count, const std::nothrow_t& tag)
{
	UNUSED_PARAMETER(tag);

	
	Ludo::ArenaAllocator<Ludo::LibCMemoryArena> allocator;
	return allocator.Alloc(count);
	

	return malloc(count);
}

void* operator new[](std::size_t count, const std::nothrow_t& tag)
{
	UNUSED_PARAMETER(tag);

	
	Ludo::ArenaAllocator<Ludo::LibCMemoryArena> allocator;
	return allocator.Alloc(count);
	

	return malloc(count);
}

void operator delete(void* ptr)
{
	free(ptr);
}

void operator delete[](void* ptr)
{
	free(ptr);
}

void operator delete(void* ptr, const std::nothrow_t& tag)
{
	UNUSED_PARAMETER(tag);

	free(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t& tag)
{
	UNUSED_PARAMETER(tag);

	free(ptr);
}
*/