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

#include "Core-Windows/Private/Platform/Helper/Errors.h"

using namespace Ludo;
using namespace Ludo::Windows;

namespace Ludo {

class SemaphoreImpl 
{
private:
	HANDLE m_Handle;

public:

	SemaphoreImpl()
	{
		m_Handle = CreateSemaphore(NULL, 0, 1000000, NULL);
		Assert(m_Handle != nullptr);
	}

	~SemaphoreImpl()
	{
		if (m_Handle != nullptr)
		{
			CloseHandle(m_Handle);
			m_Handle = nullptr;
		}		
	}

	void Signal(int Count) 
	{
		ReleaseSemaphore(m_Handle, Count, NULL);
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

Semaphore::Semaphore()
{
	CreateImpl<PlatformMemoryArena, SemaphoreImpl>();
}

Semaphore::~Semaphore()
{
	DestroyImpl<PlatformMemoryArena, SemaphoreImpl>();
}

void Semaphore::Signal(int Count)
{
	return GetImpl<SemaphoreImpl>()->Signal(Count);
}

bool Semaphore::Wait(TimeSpan Timeout)
{
	return GetImpl<SemaphoreImpl>()->Wait(Timeout);
}

}; // namespace Ludo