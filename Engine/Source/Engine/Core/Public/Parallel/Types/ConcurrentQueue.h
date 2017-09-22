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

/** \brief This type is essentially the same as a normal Queue data structure. The primary difference is that internally
 *         it uses atomic operations to ensure that operations function correctly when run in parallel.
 *
 *  \tparam ElementType Type of object the queue will contain.
 *  \tparam MaxCapacity Maximum number of elements the queue can hold at any time. Pushing more than this will result
 *                      in stalls while space is made available.
 */
template <
	typename ElementType,
	int MaxCapacity 
>
class ConcurrentQueue 
	: public INotCopyable
{
protected:
    ElementType        m_Data[MaxCapacity];
    int                m_Tail;
    int                m_UncomittedTail;
    int                m_Head;
    int                m_UncomittedHead;

public:

	/// \brief Default constructor.
	ConcurrentQueue()
		: m_Tail(0)
		, m_Head(0)
		, m_UncomittedTail(0)
		, m_UncomittedHead(0)
	{
	}

	/// \brief Destructor.
	~ConcurrentQueue()
	{
	}

	/** \brief Attempts to push a new item into the queue.
	 *
	 *  \param Element		Item to push into the queue.
	 *  \param bBlockUntilAvailable If true then the call will block until there is 
	 *                              enough space in the queue to add the new item. If
	 *                              false then the function will just return failure if 
	 *                              no space was available.
	 *
         *  \returns True if the element was able to be pushed into the queue.
	 */
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

	/** \brief Attempts to pop an item from the queue.
	 *
	 *  \param Output		Pointer to variable where we will store the poped item.
	 *  \param bBlockUntilAvailable If true then the call will block until there is 
	 *                              an item available to pop item. If false then the function 
	 *                              will just return failure if no item is available.
	 *
         *  \returns True if an element was poped from the queue successfully.
	 */
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

	/** \brief Cleans the queue of all items. This is not atomic, please only call when the queue is 
	 *         not currently being accessed.
	 */
	void Empty()
	{
		m_Top = m_Data;
		m_ComittedTop = m_Top;
	}
	
	/** \brief Gets if the queue is currently empty.
	 *
         *  \returns Returns true if the queue is currently empty.
	 */
	bool IsEmpty() const
	{
		return (m_ComittedTop == m_Data);
	}

	/** \brief Gets the number of items currently in the queue.
	 *
         *  \returns Number of items currently in the queue.
	 */
	int Length() const
	{
		return (m_ComittedTop - m_Data);
	}

};

}; // namespace Ludo
