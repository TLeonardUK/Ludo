// Copyright 2016 TwinDrills
#pragma once

#include "Core/Memory/Allocator.h"
#include "Core/Memory/ArenaAllocator.h"

#include <Windows.h>
#include <cstring>
#include <memory>

namespace Ludo {

/// \brief TODO
template <
	typename ArenaType
>
class ArenaAllocator 
	: public IAllocator
{
public:

	/// \brief TODO
	virtual void* Alloc(const int Size, const int Alignment = DefaultAlignment)
	{
		UNUSED_PARAMETER(Alignment);
		// TODO
		return malloc(Size);
	}

	/// \brief TODO
	virtual void* Realloc(void* Ptr, const int Size)
	{
		// TODO
		return realloc(Ptr, Size);
	}

	/// \brief TODO
	virtual void Free(void* Ptr) override
	{
		// TODO
		free(Ptr);
	}

};

};