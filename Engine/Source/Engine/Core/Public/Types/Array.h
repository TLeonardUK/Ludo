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

#include "Core/Public/Build/Common.h"
#include "Core/Public/Memory/ArenaAllocator.h"
#include "Core/Public/Meta/TypeConstructor.h"
#include "Core/Public/Math/Standard/Standard.h"

#include <functional>
#include <utility>
#include <initializer_list>

namespace Ludo {

/// \brief TODO
template <
	typename ElementType, 
	typename AllocatorType = ArenaAllocator<ContainerMemoryArena>
>
class Array
{
protected:
	AllocatorType	m_Allocator;
	int				m_Reserved;
	int				m_Length;
	ElementType*	m_Data;

public:

	/// \brief TODO
	typedef std::function<bool(const ElementType* A)> MatchPredicate;

	/// \brief TODO
	typedef std::function<void(const ElementType* A)> ForEachPredicate;	

	/// \brief TODO
	typedef std::function<int(const ElementType* A, const ElementType* B)> SortPredicate;

	/// \brief TODO
	static const int MinimumAllocationSize = 1;

	/// \brief TODO
	static const int NoIndex = -1;

	/// \brief TODO
	Array()
		: m_Reserved(0)
		, m_Length(0)
		, m_Data(nullptr)
	{
	}

	/// \brief TODO
	Array(std::initializer_list<ElementType> List)
		: m_Reserved(0)
		, m_Length(0)
		, m_Data(nullptr)
	{
		for (const ElementType& Type : List)
		{
			Push(Type);
		}
	}

	/// \brief TODO
	Array(const Array& Other)
		: m_Reserved(0)
		, m_Length(0)
		, m_Data(nullptr)
	{
		Append(Other.m_Data, Other.m_Length);
	}

	/// \brief TODO
	Array(const ElementType* Buffer, int Length)
		: m_Reserved(0)
		, m_Length(0)
		, m_Data(nullptr)
	{
		Append(Buffer, Length);
	}

	/// \brief TODO
	/*Array(Array&& Other)
		: m_Reserved(0)
		, m_Length(0)
		, m_Data(nullptr)
	{
		LD_UNUSED_PARAMETER(Other);
		// TODO Move.
	}*/

	/// \brief TODO
	~Array()
	{
		if (m_Data != nullptr)
		{
			// Destruct old data.
			for (int i = 0; i < m_Length; i++)
			{
				TypeConstructor<ElementType>::Destruct(m_Data[i]);
			}

			m_Allocator.Free(m_Data);
			m_Data = nullptr;
		}
	}

private:

	/// \brief TODO
	void SetLength(const int Count)
	{
		// Shrinking array.
		if (Count < m_Length)
		{
			// Call destructors on all removed items.
			for (int i = Count; i < m_Length; i++)
			{
				TypeConstructor<ElementType>::Destruct(m_Data[i]);
			}
		}

		// Expanding array. 
		else if (Count > m_Length)
		{
			// We have enough reserve memory to expand without allocation.
			if (Count > m_Reserved)
			{
				int NewSize = Math::Max(MinimumAllocationSize, Math::GetNextPowerOfTwo(Count));
				ReserveFixed(NewSize);
			}

			// Default constructor for all new items.
			for (int i = m_Length; i < Count; i++)
			{
				TypeConstructor<ElementType>::Construct(m_Data[i]);
			}
		}

		m_Length = Count;
	}

	/// \brief TODO
	void ReserveFixed(const int NewSize)
	{
		// Note: We could do a realloc here, but would mean the constructor/destructor
		//		 order would be somewhat broken, so best not to.
		ElementType* NewData = static_cast<ElementType*>(m_Allocator.Alloc(NewSize * sizeof(ElementType)));

		if (m_Data != nullptr)
		{
			// Copy over and destruct old data.
			for (int i = 0; i < m_Length; i++)
			{
				TypeConstructor<ElementType>::Construct(NewData[i]);
				NewData[i] = m_Data[i];
				TypeConstructor<ElementType>::Destruct(m_Data[i]);
			}

			// Free old data.
			m_Allocator.Free(m_Data);
		}

		// Allocate and return.
		m_Data = NewData;
		m_Reserved = NewSize;
	}

	// TODO: Might be nice to stick this all into a ISortable interface, and allow different
	//		 algorithems to be applied?

	/// \brief TODO
	int QuickSortPartition(SortPredicate Predicate, int Left, int Right)
	{
		int i = Left;
		int j = Right;
		ElementType Tmp;
		int PivotIndex = (Left + Right) / 2;
		ElementType& Pivot = m_Data[PivotIndex];

		while (i <= j)
		{
			while (Predicate(&m_Data[i], &Pivot) < 0)
			{
				i++;
			}
			while (Predicate(&m_Data[j], &Pivot) > 0)
			{
				j--;
			}
			if (i <= j)
			{
				Tmp = m_Data[i];
				m_Data[i] = m_Data[j];
				m_Data[j] = Tmp;
				i++;
				j--;
			}
		}

		return i;
	}

	/// \brief TODO
	void QuickSort(SortPredicate Predicate, int Left, int Right)
	{
		int Pivot = QuickSortPartition(Predicate, Left, Right);
		if (Left < Pivot - 1)
		{
			QuickSort(Predicate, Left, Pivot - 1);
		}
		if (Pivot < Right)
		{
			QuickSort(Predicate, Pivot, Right);
		}
	}

public:

	/// \brief TODO
	void Resize(const int Count)
	{
		SetLength(Count);
	}

	/// \brief TODO
	void Reserve(const int Count)
	{
		if (m_Reserved < Count)
		{
			ReserveFixed(Count);
		}
	}

	/// \brief TODO
	void Add(const ElementType& Element)
	{
		SetLength(m_Length + 1);
		m_Data[m_Length - 1] = Element;
	}

	/// \brief TODO
	void AddUnique(const ElementType& Element)
	{
		if (!Contains(Element))
		{
			Add(Element);
		}
	}

	/// \brief TODO
	void Insert(const Array& Array, const int Index)
	{
		Insert(Array.Data(), Array.Length(), Index);
	}

	/// \brief TODO
	void Insert(const ElementType& Element, const int Index)
	{
		LD_ASSERT(Index >= 0 && Index <= m_Length);
		SetLength(m_Length + 1);

		for (int i = m_Length - 2; i >= Index; i--)
		{
			m_Data[i + 1] = m_Data[i];
		}

		m_Data[Index] = Element;
	}

	/// \brief TODO
	void InsertUnordered(const ElementType& Element, const int Index)
	{
		LD_ASSERT(Index >= 0 && Index <= m_Length);
		SetLength(m_Length + 1);

		m_Data[m_Length - 1] = m_Data[Index];
		m_Data[Index] = Element;
	}

	/// \brief TODO
	void Insert(const ElementType* Ptr, const int Count, const int Index)
	{
		LD_ASSERT(Index >= 0 && Index <= m_Length);

		int PreviousLength = m_Length;
		SetLength(m_Length + Count);

		// Shift everything up.
		int ToShift = PreviousLength - Index;
		for (int i = 0; i < ToShift; i++)
		{
			m_Data[Index + Count + i] = m_Data[Index + i];
		}

		// Fill in data.
		for (int i = 0; i < Count; i++)
		{
			m_Data[Index + i] = Ptr[i];
		}
	}

	/// \brief TODO
	void InsertUnordered(const ElementType* Ptr, const int Count, const int Index)
	{
		LD_ASSERT(Index >= 0 && Index <= m_Length);

		int StartIndex = m_Length;
		SetLength(m_Length + Count);

		for (int i = 0; i < Count; i++)
		{
			m_Data[StartIndex + i] = m_Data[Index + i];
			m_Data[Index + i] = Ptr[i];
		}
	}

	/// \brief TODO
	void Append(const Array& Element)
	{
		int StartIndex = m_Length;
		SetLength(m_Length + Element.Length());

		for (int i = 0; i < Element.Length(); i++)
		{
			m_Data[StartIndex + i] = Element.m_Data[i];
		}
	}

	/// \brief TODO
	void Append(const ElementType* Ptr, const int Count)
	{
		int StartIndex = m_Length;
		SetLength(m_Length + Count);

		for (int i = 0; i < Count; i++)
		{
			m_Data[StartIndex + i] = Ptr[i];
		}
	}

	/// \brief TODO
	int Find(const ElementType& Element, int StartIndex = NoIndex) const
	{
		if (StartIndex == NoIndex)
		{
			StartIndex = 0;
		}

		for (int i = StartIndex; i < m_Length; i++)
		{
			if (m_Data[i] == Element)
			{
				return i;
			}
		}

		return NoIndex;
	}

	/// \brief TODO
	int FindLast(const ElementType& Element, int StartIndex = NoIndex) const
	{
		if (StartIndex == NoIndex)
		{
			StartIndex = m_Length - 1;
		}

		for (int i = StartIndex; i >= 0; i--)
		{
			if (m_Data[i] == Element)
			{
				return i;
			}
		}

		return NoIndex;
	}
	
	/// \brief TODO
	int FindMatch(MatchPredicate Predicate, int StartIndex = NoIndex) const
	{
		if (StartIndex == NoIndex)
		{
			StartIndex = 0;
		}

		for (int i = StartIndex; i < m_Length; i++)
		{
			if (Predicate(&m_Data[i]))
			{
				return i;
			}
		}

		return NoIndex;
	}

	/// \brief TODO
	int FindMatchLast(MatchPredicate Predicate, int StartIndex = NoIndex) const
	{
		if (StartIndex == NoIndex)
		{
			StartIndex = m_Length - 1;
		}

		for (int i = StartIndex; i >= 0; i--)
		{
			if (Predicate(&m_Data[i]))
			{
				return i;
			}
		}

		return NoIndex;
	}

	/// \brief TODO
	void ForEach(ForEachPredicate Predicate) const
	{
		for (int i = 0; i < m_Length; i++)
		{
			Predicate(&m_Data[i]);
		}
	}

	/// \brief TODO
	bool Contains(const ElementType& Element) const
	{
		for (int i = 0; i < m_Length; i++)
		{
			if (m_Data[i] == Element)
			{
				return true;
			}
		}

		return false;
	}

	/// \brief TODO
	void RemoveRange(const int StartIndex, const int Count)
	{
		for (int i = StartIndex; i < m_Length - Count; i++)
		{
			m_Data[i] = m_Data[i + Count];
		}
		SetLength(m_Length - Count);
	}

	/// \brief TODO
	void RemoveAt(const int Index)
	{
		for (int i = Index; i < m_Length - 1; i++)
		{
			m_Data[i] = m_Data[i + 1];
		}
		SetLength(m_Length - 1);
	}

	/// \brief TODO
	void RemoveAtUnordered(const int Index)
	{
		m_Data[Index] = m_Data[m_Length - 1];
		SetLength(m_Length - 1);
	}

	/// \brief TODO
	void Remove(const ElementType& Element)
	{
		for (int i = 0; i < m_Length; i++)
		{
			if (m_Data[i] == Element)
			{
				RemoveAt(i);
				i--;
			}
		}		
	}

	/// \brief TODO
	void RemoveUnordered(const ElementType& Element)
	{
		for (int i = 0; i < m_Length; i++)
		{
			if (m_Data[i] == Element)
			{
				RemoveAtUnordered(i);
				i--;
			}
		}
	}

	/// \brief TODO
	void RemoveMatches(MatchPredicate Predicate)
	{
		for (int i = 0; i < m_Length; i++)
		{
			if (Predicate(&m_Data[i]))
			{
				RemoveAt(i);
				i--;
			}
		}
	}

	/// \brief TODO
	void RemoveMatchesUnordered(MatchPredicate Predicate)
	{
		for (int i = 0; i < m_Length; i++)
		{
			if (Predicate(&m_Data[i]))
			{
				RemoveAtUnordered(i);
				i--;
			}
		}
	}

	/// \brief TODO
	Array Slice(const int StartIndex, const int EndIndex) const
	{
		int Len = Length();

		int FinalStartIndex = StartIndex;
		int FinalEndIndex = EndIndex;

		if (FinalStartIndex < 0)
		{
			FinalStartIndex += Len;
			if (FinalStartIndex < 0)
			{
				FinalStartIndex = 0;
			}
		}
		else if (FinalStartIndex > Len)
		{
			FinalStartIndex = Len;
		}

		if (FinalEndIndex < 0)
		{
			FinalEndIndex += Len;
		}
		else if (FinalEndIndex > Len)
		{
			FinalEndIndex = Len;
		}

		Array Result;

		if (FinalStartIndex > FinalEndIndex)
		{
			return Result;
		}
		else if (FinalStartIndex == 0 && FinalEndIndex == Len)
		{
			return *this;
		}
		else
		{
			Result.Append(m_Data + FinalStartIndex, (FinalEndIndex - FinalStartIndex));
			return Result;
		}
	}

	/// \brief TODO
	void Reverse()
	{
		int HalfSize = m_Length / 2;
		for (int i = 0; i < HalfSize; i++)
		{
			int Tmp = m_Data[i];
			m_Data[i] = m_Data[m_Length - (i + 1)];
			m_Data[m_Length - (i + 1)] = Tmp;
		}
	}

	/// \brief TODO
	void Sort(SortPredicate Predicate)
	{
		QuickSort(Predicate, 0, m_Length - 1);
	}

	/// \brief TODO
	void Push(const ElementType& Element)
	{
		Add(Element);
	}

	/// \brief TODO
	ElementType Pop(int Count = 1)
	{
		LD_ASSERT(Count <= Length());
		ElementType Result = m_Data[m_Length - Count];
		SetLength(m_Length - Count);
		return Result;
	}

	/// \brief TODO
	void Empty()
	{
		Resize(0);
	}

	/// \brief TODO
	bool IsEmpty() const
	{
		return (m_Length == 0);
	}

	/// \brief TODO
	bool IsValidIndex(const int Index) const
	{
		return (Index >= 0 && Index < m_Length);
	}

	/// \brief TODO
	int Length() const
	{
		return m_Length;
	}

	/// \brief TODO
	ElementType* First() const
	{
		if (m_Data == nullptr)
		{
			return nullptr;
		}
		return &m_Data[0];
	}

	/// \brief TODO
	ElementType* Last() const
	{
		if (m_Data == nullptr)
		{
			return nullptr;
		}
		return &m_Data[m_Length - 1];
	}

	/// \brief TODO
	ElementType* Data() const
	{
		return m_Data;
	}

	/// \brief TODO
	template <typename NewElementType> 
	Array<NewElementType, AllocatorType> CastTo()
	{
		Array<NewElementType, AllocatorType> NewArray;
		NewArray.Reserve(Length());

		for (ElementType& Element : *this)
		{
			NewArray.Add(static_cast<NewElementType>(Element));
		}

		return NewArray;
	}

	/// \brief TODO
	const ElementType& operator[](const int Index) const
	{
		LD_ASSERT(IsValidIndex(Index));
		return m_Data[Index];
	}

	/// \brief TODO
	ElementType& operator[](const int Index)
	{
        LD_ASSERT(IsValidIndex(Index));
		return m_Data[Index];
	}

	/// \brief TODO
	Array& operator =(const Array& Other)
	{
		SetLength(Other.m_Length);
		for (int i = 0; i < Other.m_Length; i++)
		{
			m_Data[i] = Other.m_Data[i];
		}

		return *this;
	}

public:

	// Do not use any of this directly, it's here purely to support std range-based for loops.

	struct StdIterator
	{
	private:
		int m_Index;
		Array* m_Array;

	public:
		StdIterator(Array* Array, int Index)
			: m_Index(Index)
			, m_Array(Array)
		{
		}

		const StdIterator& operator++()
		{
			m_Index++;
			return *this;
		}

		bool operator!=(const StdIterator& i)
		{
			return m_Index != i.m_Index;
		}

		ElementType& operator*()
		{
			return (*m_Array)[m_Index];
		}
	};

	struct ConstStdIterator
	{
	private:
		int m_Index;
		const Array* m_Array;

	public:
		ConstStdIterator(const Array* Array, int Index)
			: m_Index(Index)
			, m_Array(Array)
		{
		}

		const ConstStdIterator& operator++()
		{
			m_Index++;
			return *this;
		}

		bool operator!=(const ConstStdIterator& i)
		{
			return m_Index != i.m_Index;
		}

		const ElementType& operator*()
		{
			return (*m_Array)[m_Index];
		}
	};

	StdIterator begin()
	{
		return StdIterator(this, 0);
	}

	StdIterator end()
	{
		return StdIterator(this, m_Length);
	}

	ConstStdIterator begin() const
	{
		return ConstStdIterator(this, 0);
	}

	ConstStdIterator end() const
	{
		return ConstStdIterator(this, m_Length);
	}

};

};