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

class MutexImpl
{
private:
	CRITICAL_SECTION m_Handle;

public:

	MutexImpl()
	{
		InitializeCriticalSection(&m_Handle);
	}

	~MutexImpl()
	{
		DeleteCriticalSection(&m_Handle);
	}

	void Lock() 
	{
		EnterCriticalSection(&m_Handle);
	}

	void Unlock() 
	{
		LeaveCriticalSection(&m_Handle);
	}

	bool TryLock() 
	{
		return TryEnterCriticalSection(&m_Handle) != 0;
	}

};

Mutex::Mutex()
{
	CreateImpl<PlatformMemoryArena, MutexImpl>();
}

Mutex::~Mutex()
{
	DestroyImpl<PlatformMemoryArena, MutexImpl>();
}

void Mutex::Lock() 
{
	GetImpl<MutexImpl>()->Lock();
}

void Mutex::Unlock() 
{
	GetImpl<MutexImpl>()->Unlock();
}

bool Mutex::TryLock() 
{
	return GetImpl<MutexImpl>()->TryLock();
}

}; // namespace Ludo