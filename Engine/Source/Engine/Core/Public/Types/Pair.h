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

/// \brief TODO
template
<
	typename KeyType,
	typename ValueType
>
class Pair
{
public:

	/// \brief TODO
	Pair()
	{
	}

	/// \brief TODO
	Pair(const KeyType& Key, const ValueType& Value)
		: m_Key(Key)
		, m_Value(Value)
	{
	}

	/// \brief TODO
	inline const KeyType& Key() const
	{
		return m_Key;
	}

	/// \brief TODO
	inline const ValueType& Value() const
	{
		return m_Value;
	}

	/// \brief TODO
	inline ValueType& Value() 
	{
		return m_Value;
	}

	/// \brief TODO
	inline bool operator==(const Pair& Other) const
	{
		return m_Key == Other.m_Key && m_Value == Other.m_Value;
	}

	/// \brief TODO
	inline bool operator!=(const Pair& Other) const
	{
		return !operator==(Other);
	}

protected:
	KeyType m_Key;
	ValueType m_Value;

};

};