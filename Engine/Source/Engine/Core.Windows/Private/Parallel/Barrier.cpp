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

class BarrierImpl
{
private:
	HANDLE m_Semaphore;
	unsigned int m_Counter;
	unsigned int m_ParticipantCount;

public:

	BarrierImpl()
	{
		m_Semaphore = CreateSemaphore(NULL, 0, 1000000, NULL);
		LD_ASSERT(m_Semaphore != nullptr);
	}

	~BarrierImpl()
	{
		if (m_Semaphore != nullptr)
		{
			CloseHandle(m_Semaphore);
			m_Semaphore = nullptr;
		}
	}

	void Reset(int ParticipantCount) 
	{
		m_Counter = ParticipantCount;
		m_ParticipantCount = ParticipantCount;
	}

	void Wait() 
	{
		if (InterlockedDecrement(&m_Counter) == 0)
		{
			ReleaseSemaphore(m_Semaphore, m_ParticipantCount - 1, NULL);
		}		
		else
		{
			WaitForSingleObject(m_Semaphore, INFINITE);
		}
	}

};

// ************************************************************************************************

Barrier::Barrier()
{
	CreateImpl<PlatformMemoryArena, BarrierImpl>();
}

// ************************************************************************************************

Barrier::~Barrier()
{
	DestroyImpl<PlatformMemoryArena, BarrierImpl>();
}

// ************************************************************************************************

void Barrier::Reset(int ParticipantCount)
{
	GetImpl<BarrierImpl>()->Reset(ParticipantCount);
}

// ************************************************************************************************

void Barrier::Wait()
{
	GetImpl<BarrierImpl>()->Wait();
}

// ************************************************************************************************

}; // namespace Ludo