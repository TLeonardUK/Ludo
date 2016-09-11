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

#include <functional>
#include <utility>

namespace Ludo {

/// \brief TODO
template<typename ElementType>
class Cache
{
private:
	ElementType m_Value;
	bool m_Cached;

public:

	/// \brief TODO
	typedef std::function<ElementType(void)> GetValueFunction;

	/// \brief TODO
	Cache()
		: m_Cached(false)
	{
	}

	/// \brief TODO
	ElementType Get(GetValueFunction RetrievalFunction)
	{
		if (!m_Cached)
		{
			m_Value = RetrievalFunction();
		}
		return m_Value;
	}

	/// \brief TODO
	void Reset()
	{
		m_Cached = false;
	}

};

};