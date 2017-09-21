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

namespace Ludo {

/** \brief A memory arena represents a pool of memory from which individual allocations can be made.
 *
 * The primary purpose of arenas is to monitor and group together allocations with share a contextual or 
 * practical similarity. For example you could group all short lived strings into a small single arena,  all long
 * lived textures into a large single arena, and so forth.
 */
class IMemoryArena
{
public:
    
    /** \brief Returns the name of this arena. This should generally be something that describes what it contains.
     * 
     *  \returns Name of this arena.
     */
    virtual const char* GetName() const = 0;
    
    /** \brief Returns a description of what is contained in this arena. 
     * 
     *  \returns Description of this arena.
     */
    virtual const char* GetDescription() const = 0;
    
    /** \brief Gets the default properties this arena is initialized to on startup 
     *         if not overridden by platform configuration.
     * 
     *  \param bResizable  If true this arena can grow when it gets full, otherwise it fails when full.
     *  \param DefaultSize The default size in bytes that this arena gets initialized to.
     */
    virtual void GetDefaultState(bool& bResizable, int& DefaultSize) const = 0;
    
    /** \brief Allocates a block of memory of the given size.
     * 
     *  \param Count Size in bytes of the block of memory to allocate.
     *
     *  \returns Pointer to allocated block of memory, or nullptr on failure.
     */
    virtual void* Allocate(const int Count) = 0;
    
    /** \brief Attempts to reallocate a block of memory previously allocated by this arenas Allocate method.
     * 
     *  \param Ptr   Pointer to previously allocated memory block.
     *  \param Count New size to resize memory block to.
     *
     *  \returns Pointer to resized memory block, may be the same as the input pointer. Returns
     *           nullptr on failure.
     */
    virtual void* Reallocate(void* Ptr, const int Count) = 0;
    
    /** \brief Frees a block of memory previously allocated and returns it to the pool for future use.
     * 
     *  \param Ptr Pointer to block of memory to free.
     */
    virtual void Free(void* Ptr) = 0;

};

};
