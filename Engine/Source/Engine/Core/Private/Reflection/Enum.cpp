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

#include "Core/Public/Reflection/Enum.h"

namespace Ludo {

EnumKey::EnumKey(Enum* Base, StringId Name, int64 Value)
	: m_Enum(Base)
	, m_Name(Name)
	, m_Value(Value)
{
}

EnumKey::EnumKey()
	: m_Enum(nullptr)
	, m_Name(StringId::Empty)
	, m_Value(0)
{
}

Enum* EnumKey::GetEnum() const
{
	return m_Enum;
}

StringId EnumKey::GetName() const
{
	return m_Name;
}

int64 EnumKey::GetValue() const
{
	return m_Value;
}

Array<const EnumKey*> Enum::GetKeys()
{
	Array<const EnumKey*> Results;
	for (EnumKey& Key : m_Keys)
	{
		Results.Push(&Key);
	}
	return Results;
}

const EnumKey* Enum::FindKey(const StringId& Name)
{
	for (EnumKey& Key : m_Keys)
	{
		if (Key.GetName() == Name)
		{
			return &Key;
		}
	}
	return nullptr;
}

const EnumKey* Enum::FindKey(int64 Value)
{
	for (EnumKey& Key : m_Keys)
	{
		if (Key.GetValue() == Value)
		{
			return &Key;
		}
	}
	return nullptr;
}

}; // namespace Ludo