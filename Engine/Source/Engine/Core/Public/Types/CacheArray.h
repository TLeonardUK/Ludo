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

#include "Core/Public/Types/Cache.h"

#include <vector>

namespace Ludo {

/// \brief TODO
template<typename ElementType>
class CacheArray
{
private:
	std::vector< Cache<ElementType> > m_Values;

public:

	/// \brief TODO
	typedef std::function<ElementType(void)> GetValueFunction;

	/// \brief TODO
	CacheArray()
	{
	}

	/// \brief TODO
	ElementType Get(int Index, GetValueFunction RetrievalFunction)
	{
		if (Index >= (int)m_Values.size())
		{
			m_Values.resize(Index + 1);
		}

		return m_Values[Index].Get(RetrievalFunction);
	}

	/// \brief TODO
	void Reset()
	{
		m_Values.resize(0);
	}

};

};