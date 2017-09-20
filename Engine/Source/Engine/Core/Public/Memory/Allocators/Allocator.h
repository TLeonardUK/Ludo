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
 *
 * The primary reason for allocators is it provides a consistent way to track where our memory 
 * is dissappearing too.
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

	/** \brief 
     *
     *  \param Size
     *  \param Alignment
     *
     *  \returns
     */
	virtual void* Alloc(const int Size, const int Alignment = DefaultAlignment) = 0;
    
	/** \brief 
     *
     *  \param Ptr
     *  \param Size
     *
     *  \returns
     */
	virtual void* Realloc(void* Ptr, const int Size) = 0;
    
	/** \brief 
     *
     *  \param Ptr
     */
	virtual void Free(void* Ptr) = 0;
    
	/** \brief 
     *
     *  \tparam BaseType
     *  \tparam ConstructorTypes
     *
     *  \param Args
     *
     *  \returns
     */
	template <typename BaseType, typename... ConstructorTypes>
	BaseType* New(ConstructorTypes... Args)
	{
		void* Ptr = Alloc(sizeof(BaseType));
		return new(Ptr) BaseType(Args...);
	}
    
	/** \brief 
     *
     *  \tparam BaseType
     *
     *  \param Count
     *
     *  \returns
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
    
	/** \brief 
     *
     *  \tparam BaseType
     *
     *  \param Data
     */
	template <typename BaseType>
	void Delete(BaseType* Data)
	{
		Data->~BaseType();
		Free(Data);
	}

	/** \brief 
     *
     *  \tparam BaseType
     *
     *  \param Data
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
