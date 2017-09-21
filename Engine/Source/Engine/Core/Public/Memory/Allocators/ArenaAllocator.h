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

#include "Core/Public/Memory/Allocator.h"

namespace Ludo {

/** \brief Arena allocators allocate memory from a specific pre-defined pool of memory (known as an arena).
 * 
 *  \tparam ArenaType Class of arena, must be derived from MemoryArena.
 */
template <
	typename ArenaType
>
class ArenaAllocator 
	: public IAllocator
{
public:

    // IAllocator
    virtual void* Alloc(const int Size, const int Alignment = DefaultAlignment) override
    {
        LD_UNUSED_PARAMETER(Alignment);
        // TODO
        return malloc(Size);
    }

    /// \brief TODO
    virtual void* Realloc(void* Ptr, const int Size) override
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
    // End IAllocator

};

};
