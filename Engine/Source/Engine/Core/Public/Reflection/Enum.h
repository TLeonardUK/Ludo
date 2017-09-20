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

#include "Core/Public/Reflection/Record.h"

namespace Ludo {

class Enum;

/* 
\brief Represents an individual key defined in an Enum record.
*/
class EnumKey
	: public Record
{
private:

	Enum* m_Enum;
	int64 m_Value;

public:

	// \brief Constructors
	EnumKey();

	/*
	\brief		Gets the enum this key is contained in.
	\returns	The enum this key is contained in.
	*/
	Enum*	 GetEnum() const;

	/*
	\brief		Gets the value of this key.
	\returns	The integer value of this key.
	*/
	int64	GetValue() const;

};

/*
\brief Represents a record for an individual enum.

Contains general functionality to get individual keys and their
name and values.
*/
class Enum 
	: public Record
{
protected:

	Array<EnumKey> m_Keys;

public:

	/*
	\brief		Gets an array of all keys stored in this enum.
	\returns	Array of all keys in this enum, const as the should never be edited at runtime.
	*/
	Array<const EnumKey*> GetKeys();

	/*
	\brief		Finds a key given a hashed stringid.
	\returns	Constant pointer to key if it exists, otherwise nullptr.
	*/
	const EnumKey* FindKey(const StringId& Name);

	/*
	\brief		Finds a key given a value.
	\returns	Constant pointer to key if it exists, otherwise nullptr. If multiple keys
				exist with the same value the first will be returned.
	*/
	const EnumKey* FindKey(int64 Value);

};

}; // namespace Ludo

