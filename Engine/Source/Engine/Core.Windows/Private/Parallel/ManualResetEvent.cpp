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

#include "PCH.h"

#include "Core.Windows/Private/Platform/Helper/Errors.h"

using namespace Ludo;
using namespace Ludo::Windows;

namespace Ludo {

// ************************************************************************************************

class ManualResetEventImpl
{
private:
	HANDLE m_Handle;

public:

	ManualResetEventImpl()
	{
		m_Handle = CreateEvent(nullptr, true, false, nullptr);
		LD_ASSERT(m_Handle != nullptr);
	}

	~ManualResetEventImpl()
	{
		if (m_Handle)
		{
			CloseHandle(m_Handle);
			m_Handle = nullptr;
		}
	}

	void Reset()
	{
		ResetEvent(m_Handle);
	}

	void Signal() 
	{
		SetEvent(m_Handle);
	}

	bool Wait(TimeSpan Timeout)
	{
		DWORD Duration = (DWORD)Timeout.GetTotalMilliseconds();
		if (Timeout == TimeSpan::Infinite)
		{
			Duration = INFINITE;
		}
		return (WaitForSingleObject(m_Handle, Duration) != 0);
	}

};

// ************************************************************************************************

ManualResetEvent::ManualResetEvent()
{
	CreateImpl<PlatformMemoryArena, ManualResetEventImpl>();
}

// ************************************************************************************************

ManualResetEvent::~ManualResetEvent()
{
	DestroyImpl<PlatformMemoryArena, ManualResetEventImpl>();
}

// ************************************************************************************************

void ManualResetEvent::Reset()
{
	return GetImpl<ManualResetEventImpl>()->Reset();
}

// ************************************************************************************************

void ManualResetEvent::Signal()
{
	return GetImpl<ManualResetEventImpl>()->Signal();
}

// ************************************************************************************************

bool ManualResetEvent::Wait(TimeSpan Timeout)
{
	return GetImpl<ManualResetEventImpl>()->Wait(Timeout);
}

// ************************************************************************************************

}; // namespace Ludo