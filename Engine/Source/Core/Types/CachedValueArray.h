// Copyright 2016 TwinDrills
#pragma once

#include "Core/Types/CachedValue.h"

namespace Ludo {

/// \brief TODO
template<typename ElementType>
class CachedValueArray
{
private:
	std::vector< CachedValue<ElementType> > m_Values;

public:

	/// \brief TODO
	typedef std::function<ElementType(void)> GetValueFunction;

	/// \brief TODO
	CachedValueArray()
	{
	}

	/// \brief TODO
	ElementType Get(int Index, GetValueFunction RetrievalFunction)
	{
		if (Index >= m_Values.size())
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