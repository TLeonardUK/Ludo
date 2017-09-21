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

class Allocator;

/** \brief Replacement for new, allocates and constructs a new instance of a given
 *         type. Memory is allocated from the standard arena specified.
 * 
 *  \param Arena Type name of arena to allocate from.
 *  \param Type  Type of object to allocate.
 *  \param ...   Arguments to pass to type constructor.
 *
 *  \returns Pointer to new object, or nullptr on failure.
 */
#define LD_ARENA_NEW(Arena, Type, ...) (ArenaAllocator<Arena>().New<Type>(__VA_ARGS__))

/** \brief Replacement for new, allocates and constructs a new instance of a given
 *         type. Memory is allocated from the supplied allocator.
 * 
 *  \param Allocator Reference to allocator to allocate memory form.
 *  \param Type      Type of object to allocate.
 *  \param ...       Arguments to pass to type constructor.
 *
 *  \returns Pointer to new object, or nullptr on failure.
 */
#define LD_NEW(Allocator, Type, ...) (Allocator.New<Type>(__VA_ARGS__))

/** \brief Replacement for delete. Destructs and frees memory from an object previously 
 *         constructed by a call to LD_ARENA_NEW. 
 * 
 *  \param Arena Reference to arena the object was allocated from.
 *  \param Obj   Pointer to object to free.
 */
#define LD_ARENA_DELETE(Arena, Obj) (ArenaAllocator<Arena>().Delete(Obj))

/** \brief Replacement for delete. Destructs and frees memory from an object previously 
 *         constructed by a call to LD_NEW. 
 * 
 *  \param Arena Reference to allocator the object was allocated from.
 *  \param Obj   Pointer to object to free.
 */
#define LD_DELETE(Allocator, Obj) (Allocator.Delete(Obj))
  
/** \brief Replacement for new[], allocates and constructs a new array of the given
 *         type. Memory is allocated from the standard arena specified.
 * 
 *  \param Arena Type name of arena to allocate from.
 *  \param Type  Type of object to allocate.
 *  \param Size  Number of elements in allocated array.
 *
 *  \returns Pointer to new array, or nullptr on failure.
 */
#define LD_ARENA_NEW_ARRAY(Arena, Type, Size) (ArenaAllocator<Arena>().NewArray<Type>(Size))

/** \brief Replacement for newp[, allocates and constructs a new instance of a given
 *         type. Memory is allocated from the supplied allocator.
 * 
 *  \param Allocator Reference to allocator to allocate memory form.
 *  \param Type      Type of object to allocate.
 *  \param Size      Number of elements in allocated array.
 *
 *  \returns Pointer to new array, or nullptr on failure.
 */
#define LD_NEW_ARRAY(Allocator, Type, Size) (Allocator.NewArray<Type>(Size))

/** \brief Replacement for delete[]. Destructs and frees memory from an object previously 
 *         constructed by a call to LD_ARENA_NEW_ARRAY. 
 * 
 *  \param Arena Reference to arena the object was allocated from.
 *  \param Obj   Pointer to object to free.
 */
#define LD_ARENA_DELETE_ARRAY(Arena, Obj) (ArenaAllocator<Arena>().DeleteArray(Obj))

/** \brief Replacement for delete[]. Destructs and frees memory from an object previously 
 *         constructed by a call to LD_NEW_ARRAY. 
 * 
 *  \param Arena Reference to allocator the object was allocated from.
 *  \param Obj   Pointer to object to free.
 */
#define LD_DELETE_ARRAY(Allocator, Obj) (Allocator.DeleteArray(Obj))

}; // namespace Ludo
