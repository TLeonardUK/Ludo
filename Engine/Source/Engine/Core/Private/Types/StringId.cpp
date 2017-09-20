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

#include "PCH.h"

#include "Core/Public/Types/StringId.h"

namespace Ludo {

StringId StringId::Empty(0u, 0);

Map<unsigned int, int> StringId::m_StringIndexLookup;
Array<String> StringId::m_StringArray;

// ************************************************************************************************

StringId::StringId()
	: m_Hash(0)
	, m_Index(-1)
{
}

// ************************************************************************************************

StringId::StringId(unsigned int Hash, int Index)
	: m_Hash(Hash)
	, m_Index(Index)
{
}

// ************************************************************************************************

bool StringId::operator==(const StringId& Other) const
{
	return m_Index == Other.m_Index;
}

// ************************************************************************************************

bool StringId::operator!=(const StringId& Other) const
{
	return m_Index != Other.m_Index;
}

// ************************************************************************************************

String StringId::ToString() const
{
	return Lookup(*this);
}

// ************************************************************************************************

unsigned int StringId::GetHash() const
{
	return m_Hash;
}

// ************************************************************************************************

StringId StringId::Create(const String& Value)
{
	unsigned int Hash = Value.ToHash();
	int Index = -1;

	// Keep incrementing hash if the current one produces a hash collision
	// with a different string.
	while (true)
	{
		if (!m_StringIndexLookup.FindValue(Hash, Index))
		{
			m_StringArray.Push(Value);
			Index = m_StringArray.Length() - 1;
			m_StringIndexLookup.Set(Hash, Index);
			break;
		}
		else
		{
			if (m_StringArray[Index] == Value)
			{
				break;
			}
			else
			{
				Hash++;
			}
		}
	}

	return StringId(Hash, Index);
}

// ************************************************************************************************

StringId StringId::Create(const char* Value)
{
	return Create(String(Value));
}

// ************************************************************************************************

String StringId::Lookup(const StringId& Id)
{
	if (Id.m_Index < 0)
	{
		return "";
	}

	return m_StringArray[Id.m_Index];
}

// ************************************************************************************************

}; // namespace Ludo