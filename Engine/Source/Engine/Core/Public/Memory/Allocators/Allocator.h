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

/** \brief Base class for all allocators.
 *
 * Allocators are responsible for allocating and freeing memory from different sources
 * or with different methods.
 *
 * All allocations should go through an allocator, new/delete/malloc/free etc should never
 * be called directly.
 */
class IAllocator 
{
private:
	struct ArrayHeader
	{
		int Size;
	};

public:

    /// \brief The default alignment all allocations should have their addresses aligned to.
    static const int DefaultAlignment = -1;

    /** \brief Allocates a block of memory with the given size and alignment.
     *
     *  \param Size      Size of block in bytes to allocate.
     *  \param Alignment Alignment to align memory pointer to.
     *
     *  \returns Pointer to allocated block of memory, or nullptr on failure.
     */
    virtual void* Alloc(const int Size, const int Alignment = DefaultAlignment) = 0;
    
    /** \brief Attempts to reallocate a block of memory to a different size.
     *
     *  \param Ptr   Pointer to block of memory to reallocate.
     *  \param Size  New size, in bytes, to reallocate block of memory to.
     *
     *  \returns Pointer to newly reallocated block of memory, this may or may not 
     *           be the same as the original pointer. Returns nullptr on failure.
     */
    virtual void* Realloc(void* Ptr, const int Size) = 0;
    
    /** \brief Deallocates a block of memory previously allocated by this allocator.
     *
     *  \param Ptr Pointer to block of memory to free.
     */
    virtual void Free(void* Ptr) = 0;
    
    /** \brief Allocates memory and calls the constructor for an object of a given type.
     *
     *  \tparam BaseType	 Type of object to construct.
     *  \tparam ConstructorTypes Type of constructor arguments. Not usually explicitly defined, 
     *                           but automatically resolved by the compiler.
     *
     *  \param Args		 Arguments to pass to the constructor.
     *
     *  \returns		 Constructed instance of class, or nullptr of failure.
     */
    template <typename BaseType, typename... ConstructorTypes>
    BaseType* New(ConstructorTypes... Args)
    {
        void* Ptr = Alloc(sizeof(BaseType));
        return new(Ptr) BaseType(Args...);
    }
    
    /** \brief Allocates memory and calls the constructor for an array of objects of a given type.
     *
     *  \tparam BaseType Type of object to construct.
     *
     *  \param Count Size of array to allocate.
     *
     *  \returns Pointer to array of objects of the given size.
     */
    template <typename BaseType>
    BaseType* NewArray(int Count)
    {
        void* Ptr = Alloc(sizeof(ArrayHeader) + sizeof(BaseType) * Count);
        
        ArrayHeader* Header = (ArrayHeader*)Ptr;
        Header->Size = Count;
        
        BaseType* Array = (BaseType*)(char*)(Header + 1);
        
        for (int i = 0; i < Count; i++)
        {
            new (Array + i) BaseType();
        }
        
        return Array;
    }
    
    /** \brief Calls destructor and frees memory for object previously allocated with New.
     *
     *  \tparam BaseType Type of object to destruct. Usually resolved by the compiler.
     *
     *  \param Data Pointer to object to delete.
     */
    template <typename BaseType>
    void Delete(BaseType* Data)
    {
        Data->~BaseType();
        Free(Data);
    }

    /** \brief Calls the destructor for each object in an array and frees the memory for the array.
     *
     *  \tparam BaseType Base type of array to destruct. Usually resolved by the compiler.
     *
     *  \param Data Pointer to array of objects to delete.
     */
    template <typename BaseType>
    void DeleteArray(BaseType* Data)
    {
        ArrayHeader* Header = ((ArrayHeader*)Data) - 1;
        
        for (int i = 0; i < Header->Size; i++)
        {
            Data[i].~BaseType();
        }

        Free(Header);
    }

};

};
