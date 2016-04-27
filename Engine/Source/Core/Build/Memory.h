// Copyright 2016 TwinDrills
#pragma once

// This file contains all the default memory configurations.

#include "Core/Memory/MemoryArena.h"
#include "Core/Platform/Memory.h"

namespace Ludo {

/// \brief TODO
class LibCMemoryArena : public IMemoryArena
{
public:
	const char* GetName() const override
	{
		return "LibC Memory Arena";
	}
	const char* GetDescription() const override
	{
		return "Memory arena that all libc allocations are redirected to, unless otherwise overriden.";
	}
	void GetDefaultState(bool& bResizable, int& DefaultSize) const override
	{
		Memory::GetArenaDefaultState(Memory::StandardMemoryArena::LibC, bResizable, DefaultSize);
	}
	void* Allocate(const int Count) override
	{
		return Memory::Allocate(Count, Memory::MemoryLocationHint::CPU);
	}
	void* Reallocate(void* Ptr, const int Count) override
	{
		return Memory::Reallocate(Ptr, Count);
	}
	void Free(void* Ptr) override
	{
		return Memory::Free(Ptr);
	}
};

/// \brief TODO
class ContainerMemoryArena : public IMemoryArena
{
public:
	const char* GetName() const override
	{
		return "Core Container Memory Arena";
	}
	const char* GetDescription() const override 
	{
		return "Memory arena that is used by all core containers, unless otherwise overridden.";
	}
	void GetDefaultState(bool& bResizable, int& DefaultSize) const override
	{
		Memory::GetArenaDefaultState(Memory::StandardMemoryArena::Containers, bResizable, DefaultSize);
	}
	void* Allocate(const int Count) override
	{
		return Memory::Allocate(Count, Memory::MemoryLocationHint::CPU);
	}
	void* Reallocate(void* Ptr, const int Count) override
	{
		return Memory::Reallocate(Ptr, Count);
	}
	void Free(void* Ptr) override
	{
		return Memory::Free(Ptr);
	}
};

/// \brief TODO
class StringMemoryArena : public IMemoryArena
{
public:
	const char* GetName() const override
	{
		return "String Memory Arena";
	}
	const char* GetDescription() const override
	{
		return "Memory arena that is used by all strings, unless otherwise overridden.";
	}
	void GetDefaultState(bool& bResizable, int& DefaultSize) const override
	{
		Memory::GetArenaDefaultState(Memory::StandardMemoryArena::Strings, bResizable, DefaultSize);
	}
	void* Allocate(const int Count) override
	{
		return Memory::Allocate(Count, Memory::MemoryLocationHint::CPU);
	}
	void* Reallocate(void* Ptr, const int Count) override
	{
		return Memory::Reallocate(Ptr, Count);
	}
	void Free(void* Ptr) override
	{
		return Memory::Free(Ptr);
	}
};

/// \brief TODO
class PlatformMemoryArena : public IMemoryArena
{
public:
	const char* GetName() const override
	{
		return "Platform Memory Arena";
	}
	const char* GetDescription() const override
	{
		return "Memory arena that is used by all platform specific classes (Threads, etc), unless otherwise overridden.";
	}
	void GetDefaultState(bool& bResizable, int& DefaultSize) const override
	{
		Memory::GetArenaDefaultState(Memory::StandardMemoryArena::Platform, bResizable, DefaultSize);
	}
	void* Allocate(const int Count) override
	{
		return Memory::Allocate(Count, Memory::MemoryLocationHint::CPU);
	}
	void* Reallocate(void* Ptr, const int Count) override
	{
		return Memory::Reallocate(Ptr, Count);
	}
	void Free(void* Ptr) override
	{
		return Memory::Free(Ptr);
	}
};

}; // nanmespace Ludo