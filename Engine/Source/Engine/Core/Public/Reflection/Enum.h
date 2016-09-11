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

#include "Core/Public/Reflection/Type.h"

namespace Ludo {

class Enum;

/// \brief TODO
class EnumKey
{
private:
	friend class ReflectionFileScanner;

	StringId m_Name;
	Enum* m_Enum;
	int64 m_Value;

public:
	EnumKey();
	EnumKey(Enum* Base, StringId Name, int64 Value);

	Enum*	 GetEnum() const;
	StringId GetName() const;
	int64	GetValue() const;

};

/// \brief TODO
class Enum : public Type
{
protected:
	friend class ReflectionFileScanner;

	Array<EnumKey> m_Keys;

public:
	Array<const EnumKey*> GetKeys();

	const EnumKey* FindKey(const StringId& Name);
	const EnumKey* FindKey(int64 Value);

};

}; // namespace Ludo

