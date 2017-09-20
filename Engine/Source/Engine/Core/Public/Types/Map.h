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

#include "Core/Public/Memory/ArenaAllocator.h"
#include "Core/Public/Meta/TypeConstructor.h"
#include "Core/Public/Types/String.h"
#include "Core/Public/Types/Array.h"
#include "Core/Public/Types/Pair.h"

#include <functional>
#include <utility>

namespace Ludo {

/// \brief TODO
template <typename KeyType>
struct MapHashGenerator
{
	static unsigned int GetHash(const KeyType& Value)
	{
		// If its not a default supported type, we assume it has a hashing function. If it
		// dosen't the user should either define one for the object or pass the Map struct
		// a different HashGenerator type.
		return Value.GetHash();
	}
};

template <typename KeyType>
struct MapHashGenerator<KeyType*>
{
	static unsigned int GetHash(const KeyType& Value)
	{
		// If its not a default supported type, we assume it has a hashing function. If it
		// dosen't the user should either define one for the object or pass the Map struct
		// a different HashGenerator type.
		return Value->GetHash();
	}
};

template <>
struct MapHashGenerator<const char*>
{
	static unsigned int GetHash(const char*& Value)
	{
		return String(Value).ToHash();
	}
};

template <>
struct MapHashGenerator<String>
{
	static unsigned int GetHash(const String& Value)
	{
		return Value.ToHash();
	}
};

template <>
struct MapHashGenerator<int>
{
	static unsigned int GetHash(const int& Value)
	{
		return *reinterpret_cast<const unsigned int*>(&Value);
	}
};

template <>
struct MapHashGenerator<unsigned int>
{
	static unsigned int GetHash(const unsigned int& Value)
	{
		return Value;
	}
};

/// \brief TODO
template <
	typename KeyType,
	typename ValueType,
	typename AllocatorType = ArenaAllocator<ContainerMemoryArena>,
	typename HashGenerator = MapHashGenerator<KeyType>
>
class Map
{
public:

	/// \brief TODO
	typedef Pair<KeyType, ValueType> PairType;

	/// \brief TODO
	typedef std::function<void(const PairType& Pair)> ForEachPredicate;

	/// \brief TODO
	typedef std::function<bool(const PairType& Pair)> MatchPredicate;

protected:

	/// \brief TODO
	static const int StaticBucketCount = 16;

	/// \brief TODO
	const float BucketLoadReallocationThreshold = 0.66f;

	/// \brief TODO
	const float BucketReallocationSizeMultiplier = 2.0f;

	struct Bucket
	{
		Array<PairType> Values;
	};

	AllocatorType	m_Allocator;
	Bucket*			m_Buckets;
	Bucket			m_StaticBuckets[StaticBucketCount];
	int				m_BucketCount;
	int				m_TotalEntries;

public:

	/// \brief TODO
	Map()
		: m_Buckets(m_StaticBuckets)
		, m_BucketCount(StaticBucketCount)
		, m_TotalEntries(0)
	{
	}

	/// \brief TODO
	Map(const Map& Other)
		: m_Buckets(m_StaticBuckets)
		, m_BucketCount(StaticBucketCount)
		, m_TotalEntries(0)
	{
		Other.CopyTo(*this);
	}

	/// \brief TODO
	/*Map(Map&& Other)
	{
	LD_UNUSED_PARAMETER(Other);
	// TODO Move.
	}*/

	/// \brief TODO
	~Map()
	{
		if (m_Buckets != nullptr)
		{
			if (m_Buckets != m_StaticBuckets)
			{
				m_Allocator.DeleteArray(m_Buckets);
				m_Buckets = nullptr;
			}
		}
	}

private:

	void CopyTo(Map& Other) const
	{
		for (int i = 0; i < m_BucketCount; i++)
		{
			Bucket& Bucket = m_Buckets[i];
			for (int j = 0; j < Bucket.Values.Length(); j++)
			{
				Pair<KeyType, ValueType>& Entry = Bucket.Values[j];
				Other.Set(Entry.Key(), Entry.Value());
			}
		}
	}

	unsigned int GetHash(const KeyType& Key) const
	{
		return HashGenerator::GetHash(Key);		
	}

	void ResizeBuckets(int Count)
	{
		Bucket* OldBuckets = m_Buckets;
		int OldBucketCount = m_BucketCount;
		int OldTotalEntries = m_TotalEntries;

		m_BucketCount = Count;
		m_Buckets = m_Allocator.NewArray<Bucket>(Count);

		for (int i = 0; i < OldBucketCount; i++)
		{
			for (int j = 0; j < OldBuckets[i].Values.Length(); j++)
			{
				PairType& Value = OldBuckets[i].Values[j];
				Set(Value.Key(), Value.Value());
			}
			OldBuckets[i].Values.Empty();
		}

		m_TotalEntries = OldTotalEntries;

		if (OldBuckets != m_StaticBuckets)
		{
			m_Allocator.DeleteArray(OldBuckets);
		}
	}

public:

	/// \brief TODO
	void Set(const KeyType& Key, const ValueType& Value)
	{
		unsigned int Hash = GetHash(Key);
		int Bucket = Hash % m_BucketCount;
		for (int i = 0; i < m_Buckets[Bucket].Values.Length(); i++)
		{
			if (m_Buckets[Bucket].Values[i].Key() == Key)
			{
				m_Buckets[Bucket].Values[i] = PairType(Key, Value);
				return;
			}
		}

		m_Buckets[Bucket].Values.Push(PairType(Key, Value));
		m_TotalEntries++;

		float LoadFactor = (float)m_TotalEntries / (float)m_BucketCount;
		if (LoadFactor >= BucketLoadReallocationThreshold)
		{
			ResizeBuckets((int)(m_BucketCount * BucketReallocationSizeMultiplier));
		}
	}

	/// \brief TODO
	Array<KeyType> GetKeys() const
	{
		Array<KeyType> Keys;

		for (int i = 0; i < m_BucketCount; i++)
		{
			for (int j = 0; j < m_Buckets[i].Values.Length(); j++)
			{
				Keys.Add(m_Buckets[i].Values[j].Key());
			}
		}

		return Keys;
	}

	/// \brief TODO
	Array<ValueType> GetValues() const
	{
		Array<ValueType> Values;

		for (int i = 0; i < m_BucketCount; i++)
		{
			for (int j = 0; j < m_Buckets[i].Values.Length(); j++)
			{
				Values.Add(m_Buckets[i].Values[j].Value());
			}
		}

		return Values;
	}

	/// \brief TODO
	Array<PairType> GetPairs() const
	{
		Array<PairType> Pairs;

		for (int i = 0; i < m_BucketCount; i++)
		{
			for (int j = 0; j < m_Buckets[i].Values.Length(); j++)
			{
				Pairs.Add(m_Buckets[i].Values[j]);
			}
		}

		return Pairs;
	}

	/// \brief TODO
	bool FindKey(const ValueType& Value, KeyType& Key) const
	{
		for (int i = 0; i < m_BucketCount; i++)
		{
			for (int j = 0; j < m_Buckets[i].Values.Length(); j++)
			{
				if (m_Buckets[i].Values[j].Value() == Value)
				{
					Key = m_Buckets[i].Values[j].Key();
					return true;
				}
			}
		}

		return false;
	}

	/// \brief TODO
	bool FindValue(const KeyType& Key, ValueType& Value) const
	{
		unsigned int Hash = GetHash(Key);
		int Bucket = Hash % m_BucketCount;

		for (int i = 0; i < m_Buckets[Bucket].Values.Length(); i++)
		{
			if (m_Buckets[Bucket].Values[i].Key() == Key)
			{
				Value = m_Buckets[Bucket].Values[i].Value();
				return true;
			}
		}

		return false;
	}

	/// \brief TODO
	void ForEach(ForEachPredicate Predicate) const
	{
		for (int i = 0; i < m_BucketCount; i++)
		{
			for (int j = 0; j < m_Buckets[i].Values.Length(); j++)
			{
				Predicate(m_Buckets[i].Values[j]);
			}
		}
	}

	/// \brief TODO
	bool ContainsKey(const KeyType& Key) const
	{
		unsigned int Hash = GetHash(Key);
		int Bucket = Hash % m_BucketCount;

		for (int i = 0; i < m_Buckets[Bucket].Values.Length(); i++)
		{
			if (m_Buckets[Bucket].Values[i].Key() == Key)
			{
				return true;
			}
		}

		return false;
	}

	/// \brief TODO
	bool ContainsValue(const ValueType& Value) const
	{
		for (int i = 0; i < m_BucketCount; i++)
		{
			for (int j = 0; j < m_Buckets[i].Values.Length(); j++)
			{
				if (m_Buckets[i].Values[j].Value() == Value)
				{
					return true;
				}
			}
		}

		return false;
	}

	/// \brief TODO
	void RemoveKey(const KeyType& Key)
	{
		unsigned int Hash = GetHash(Key);
		int Bucket = Hash % m_BucketCount;

		for (int i = 0; i < m_Buckets[Bucket].Values.Length(); i++)
		{
			if (m_Buckets[Bucket].Values[i].Key() == Key)
			{
				m_Buckets[Bucket].Values.RemoveAtUnordered(i);
				m_TotalEntries--;
				break;
			}
		}
	}

	/// \brief TODO
	void RemoveValue(const ValueType& Value)
	{
		for (int i = 0; i < m_BucketCount; i++)
		{
			for (int j = 0; j < m_Buckets[i].Values.Length(); j++)
			{
				if (m_Buckets[i].Values[j].Value() == Value)
				{
					m_Buckets[i].Values.RemoveAtUnordered(j);
					m_TotalEntries--;
					break;
				}
			}
		}	
	}

	/// \brief TODO
	void RemoveMatches(MatchPredicate Predicate)
	{
		for (int i = 0; i < m_BucketCount; i++)
		{
			for (int j = 0; j < m_Buckets[i].Values.Length(); j++)
			{
				if (Predicate(m_Buckets[i].Values[j]))
				{
					m_Buckets[i].Values.RemoveAtUnordered(j);
					m_TotalEntries--;
					j--;
					continue;
				}
			}
		}
	}

	/// \brief TODO
	void Empty()
	{
		for (int i = 0; i < m_BucketCount; i++)
		{
			m_Buckets[i].Values.Empty();			
		}
		m_TotalEntries = 0;
	}

	/// \brief TODO
	bool IsEmpty() const
	{
		return (m_TotalEntries == 0);
	}

	/// \brief TODO
	int Count() const
	{
		return m_TotalEntries;
	}

	/// \brief TODO
	const ValueType operator[](const KeyType& Key) const
	{
		unsigned int Hash = GetHash(Key);
		int Bucket = Hash % m_BucketCount;

		for (int i = 0; i < m_Buckets[Bucket].Values.Length(); i++)
		{
			if (m_Buckets[Bucket].Values[i].Key() == Key)
			{
				return m_Buckets[Bucket].Values[i].Value();
			}
		}

		LD_CONSTANT_ASSERT_MSG("Attempted to index constant map with unknown key.");		

		static ValueType invalid_type;
		return invalid_type;
	}

	/// \brief TODO
	ValueType& operator[](const KeyType& Key)
	{
		unsigned int Hash = GetHash(Key);
		int Bucket = Hash % m_BucketCount;

		for (int i = 0; i < m_Buckets[Bucket].Values.Length(); i++)
		{
			if (m_Buckets[Bucket].Values[i].Key() == Key)
			{
				return m_Buckets[Bucket].Values[i].Value();
			}
		}

		LD_CONSTANT_ASSERT_MSG("Attempted to index constant map with unknown key.");

		static ValueType invalid_type;
		return invalid_type;
	}

	/// \brief TODO
	Map& operator =(const Map& Other)
	{
		Other.CopyTo(*this);
		return *this;
	}

public:

	// Do not use any of this directly, it's here purely to support std range-based for loops.

	template <typename IterType, typename MapType, typename ValueType>
	struct BaseIterator
	{
	private:
		int m_BucketIndex;
		int m_ValueIndex;
		MapType* m_Map;

	public:
		BaseIterator(MapType* Map, int BucketIndex, int ValueIndex)
			: m_Map(Map)
			, m_BucketIndex(BucketIndex)
			, m_ValueIndex(ValueIndex)
		{
		}

		const IterType& operator++()
		{
			if (m_BucketIndex < m_Map->m_BucketCount)
			{
				m_ValueIndex++;
				if (m_ValueIndex >= m_Map->m_Buckets[m_BucketIndex].Values.Length())
				{
					m_ValueIndex = 0;
					while (true)
					{
						m_BucketIndex++;

						if (m_BucketIndex >= m_Map->m_BucketCount ||
							m_Map->m_Buckets[m_BucketIndex].Values.Length() > 0)
						{
							break;
						}
					}
				}
			}
			return *static_cast<IterType*>(this);
		}

		bool operator!=(const IterType& i)
		{
			return m_Map != i.m_Map || m_BucketIndex != i.m_BucketIndex || m_ValueIndex != i.m_ValueIndex;
		}

		ValueType& operator*()
		{
			return m_Map->m_Buckets[m_BucketIndex].Values[m_ValueIndex];
		}
	};

	struct Iterator : public BaseIterator<Iterator, Map, PairType> 
	{
		Iterator(Map* Map, int BucketIndex, int ValueIndex)
			: BaseIterator(Map, BucketIndex, ValueIndex)
		{
		}
	};

	struct ConstIterator : public BaseIterator<ConstIterator, const Map, const PairType>
	{
		ConstIterator(const Map* Map, int BucketIndex, int ValueIndex)
			: BaseIterator(Map, BucketIndex, ValueIndex)
		{
		}
	};


	Iterator begin()
	{
		int BucketIndex = 0;

		while (BucketIndex < m_BucketCount)
		{
			Bucket& Bucket = m_Buckets[BucketIndex];
			if (Bucket.Values.Length() > 0)
			{
				break;
			}
			else
			{
				BucketIndex++;
			}
		}

		return Iterator(this, BucketIndex, 0);
	}

	Iterator end()
	{
		return Iterator(this, m_BucketCount, 0);
	}

	ConstIterator begin() const
	{
		int BucketIndex = 0;

		while (BucketIndex < m_BucketCount)
		{
			Bucket& Bucket = m_Buckets[BucketIndex];
			if (Bucket.Values.Length() > 0)
			{
				break;
			}
			else
			{
				BucketIndex++;
			}
		}

		return ConstIterator(this, 0, 0);
	}

	ConstIterator end() const
	{
		return ConstIterator(this, m_BucketCount, 0);
	}
}; 

}; // namespace Ludo