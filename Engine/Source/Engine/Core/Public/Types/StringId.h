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

#include "Core/Public/Types/String.h"
#include "Core/Public/Types/Map.h"

namespace Ludo {

/// \brief TODO
class StringId
{
public:

	/// \brief TODO
	StringId();

	/// \brief TODO
	bool operator==(const StringId& Other) const;

	/// \brief TODO
	bool operator!=(const StringId& Other) const;

	/// \brief TODO
	String ToString() const;

	/// \brief TODO
	unsigned int GetHash() const;

	/// \brief TODO
	static StringId Create(const String& Value);

	/// \brief TODO
	static StringId Create(const char* Value);

	/// \brief TODO
	static String Lookup(const StringId& Id);

	/// \brief TODO
	static StringId Empty;

protected:

	/// \brief TODO
	StringId(unsigned int Hash, int Index);

	unsigned int m_Hash;
	int m_Index;

	static Map<unsigned int, int> m_StringIndexLookup;
	static Array<String> m_StringArray;

};

};