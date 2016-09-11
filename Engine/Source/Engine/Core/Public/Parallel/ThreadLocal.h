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

#include "Core/Public/Time/Time.h"
#include "Core/Public/Meta/IPimpl.h"
#include "Core/Public/Meta/Traits/INotCopyable.h"

namespace Ludo {

/// \brief TODO
int NewTlsSlot();

/// \brief TODO
void FreeTlsSlot(int Slot);

/// \brief TODO
void SetTlsSlot(int Slot, void* Data);

/// \brief TODO
void* GetTlsSlot(int Slot);

/// \brief TODO
template <typename T>
class ThreadLocal :
	public INotCopyable
{
public:

	static_assert(sizeof(T) <= sizeof(void*), "TLS data must have an equal or lower size than the platform pointer width.");

	/// \brief TODO
	ThreadLocal()
	{
		m_TlsSlot = NewTlsSlot();
	}

	/// \brief TODO
	~ThreadLocal()
	{
		FreeTlsSlot(m_TlsSlot);
	}

	/// \brief TODO
	void Set(T Value)
	{
		SetTlsSlot(m_TlsSlot, reinterpret_cast<char*>(Value));
	}
	
	/// \brief TODO
	T Get()
	{
		void* Data = SetTlsSlot(m_TlsSlot);
		return reinterpret_cast<T>(reinterpret_cast<char*>(Data));
	}

};

}; // namespace Ludo