// Copyright 2016 TwinDrills
#pragma once

#include <functional>
#include <utility>

namespace Ludo {

/// \brief TODO
template<typename ElementType>
class CachedValue
{
private:
	ElementType m_Value;
	bool m_Cached;

public:

	/// \brief TODO
	typedef std::function<ElementType(void)> GetValueFunction;

	/// \brief TODO
	CachedValue()
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