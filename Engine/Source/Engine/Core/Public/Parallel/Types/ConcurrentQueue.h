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

#include "Core/Public/Memory/ArenaAllocator.h"
#include "Core/Public/Meta/TypeConstructor.h"
#include "Core/Public/Meta/Traits/INotCopyable.h"
#include "Core/Public/Parallel/Atomic.h"
#include "Core/Public/Math/Standard/Standard.h"

#include <functional>
#include <utility>
#include <initializer_list>

namespace Ludo {

/// \brief TODO
template <
	typename ElementType,
	int MaxCapacity 
>
class ConcurrentQueue 
	: public INotCopyable
{
protected:
	ElementType		m_Data[MaxCapacity];
	int				m_Tail;
	int				m_UncomittedTail;
	int				m_Head;
	int				m_UncomittedHead;

public:

	/// \brief TODO
	ConcurrentQueue()
		: m_Tail(0)
		, m_Head(0)
		, m_UncomittedTail(0)
		, m_UncomittedHead(0)
	{
	}

	/// \brief TODO
	~ConcurrentQueue()
	{
	}

	/// \brief TODO
	bool Push(const ElementType& Element, bool bBlockUntilAvailable = false)
	{
		while (true)
		{
			int OldHead = m_Head;
			int NewHead = OldHead + 1;

			int Diff = (NewHead - m_Tail);
			if (Diff < 0 || Diff > MaxCapacity)
			{
				if (bBlockUntilAvailable)
				{
					continue;
				}
				else
				{
					break;
				}
			}

			int Result = Atomic::CompareExchange(&m_UncomittedHead, NewHead, OldHead);
			if (Result == OldHead)
			{
				m_Data[OldHead % MaxCapacity] = Element;
				m_Head = m_UncomittedHead;
				return true;
			}
			else
			{
				Atomic::RelaxCpu();
			}
		}
		return false;
	}

	/// \brief TODO
	bool Pop(ElementType* Output, bool bBlockUntilAvailable = false)
	{
		while (true)
		{
			int OldTail = m_Tail;
			int NewTail = OldTail + 1;

			int Diff = (m_Head - NewTail);
			if (Diff < 0 || Diff > MaxCapacity)
			{
				if (bBlockUntilAvailable)
				{
					continue;
				}
				else
				{
					break;
				}
			}

			int Result = Atomic::CompareExchange(&m_UncomittedTail, NewTail, OldTail);
			if (Result == OldTail)
			{
				*Output = m_Data[OldTail % MaxCapacity];
				m_Tail = m_UncomittedTail;
				return true;
			}
			else
			{
				Atomic::RelaxCpu();
			}
		}
		return false;
	}

	/// \brief TODO
	void Empty()
	{
		m_Top = m_Data;
		m_ComittedTop = m_Top;
	}

	/// \brief TODO
	bool IsEmpty() const
	{
		return (m_ComittedTop == m_Data);
	}

	/// \brief TODO
	int Length() const
	{
		return (m_ComittedTop - m_Data);
	}

};

}; // namespace Ludo