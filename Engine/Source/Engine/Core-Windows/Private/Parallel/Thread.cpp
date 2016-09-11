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

__declspec(thread) Thread* g_ThreadSelf;
				   Thread* g_ThreadMain;

Array<Thread*> g_nonEngineThreads;
CRITICAL_SECTION g_nonEngineThreadsCS;

class ThreadImpl 
{
private:
	Thread::EntryPoint m_EntryPoint;
	HANDLE m_Thread;
	DWORD m_ThreadId;
	String m_Name;
	bool m_Finished;
	bool m_Aborted;
	bool m_Owned;
	Thread* m_Owner;

public:
	ThreadImpl()
		: m_EntryPoint(nullptr)
		, m_Thread(INVALID_HANDLE_VALUE)
		, m_ThreadId(0)
		, m_Name("")
		, m_Finished(false)
		, m_Aborted(false)
		, m_Owner(nullptr)
	{
	}

	~ThreadImpl()
	{
		if (m_Owned && m_Thread != nullptr)
		{
			Join();

			CloseHandle(m_Thread);
			m_Thread = nullptr;
		}
	}

	void CreateOwnedThread()
	{
		m_Thread = CreateThread(
			NULL,
			NULL,
			ThreadThunk,
			this,
			CREATE_SUSPENDED,
			&m_ThreadId
		);

		if (m_Thread == INVALID_HANDLE_VALUE)
		{
			AssertLastSystemError("CreateThread");
		}

		m_Owned = true;
	}

	void SetOwner(Thread* Owner)
	{
		m_Owner = Owner;
	}

	void SetHandles(HANDLE Thread, DWORD ThreadId)
	{
		m_Owned = false;
		m_Thread = Thread;
		m_ThreadId = ThreadId;
	}

	void SetName(const String& Name) 
	{
		SetVsThreadName(m_ThreadId, Name.Data());
	}

	void Start(Thread::EntryPoint Point) 
	{
		m_EntryPoint = Point;
		ResumeThread(m_Thread);
	}

	void Abort() 
	{
		m_Aborted = true;
	}

	void Join() 
	{
		WaitForSingleObject(m_Thread, INFINITE);
	}

	bool IsRunning() 
	{
		return !m_Finished;
	}

	bool IsAborted() 
	{
		return m_Aborted;
	}

	bool IsEqual(const ThreadImpl* other) const
	{
		return m_ThreadId == other->m_ThreadId;
	}

	// This lovely struct + function are used to set the name of a thread
	// in the visual studio debugger. Only works in visual studio!
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType; // must be 0x1000
		LPCSTR szName; // pointer to name (in user addr space)
		DWORD dwThreadID; // thread ID (-1=caller thread)
		DWORD dwFlags; // reserved for future use, must be zero
	} THREADNAME_INFO;

	static void SetVsThreadName(DWORD dwThreadID, const char* szThreadName)
	{
		if (IsDebuggerPresent())
		{
			THREADNAME_INFO info;
			info.dwType = 0x1000;
			info.szName = szThreadName;
			info.dwThreadID = dwThreadID;
			info.dwFlags = 0;

			__try
			{
				RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (ULONG_PTR*)&info);
			}
			__except (EXCEPTION_CONTINUE_EXECUTION)
			{
			}
		}
	}

	static DWORD WINAPI ThreadThunk(LPVOID lpThreadParameter)
	{
		ThreadImpl* Impl = reinterpret_cast<ThreadImpl*>(lpThreadParameter);

		g_ThreadSelf = Impl->m_Owner;

		// Call entry point.
		Impl->m_EntryPoint(g_ThreadSelf);
		Impl->m_Finished = true;

		g_ThreadSelf = nullptr;

		return 0;
	}

};
 
Thread::Thread()
{
	CreateImpl<PlatformMemoryArena, ThreadImpl>();
	GetImpl<ThreadImpl>()->SetOwner(this);
	GetImpl<ThreadImpl>()->CreateOwnedThread();
}

Thread::Thread(bool bCreateOwnThread)
{
	CreateImpl<PlatformMemoryArena, ThreadImpl>();
	GetImpl<ThreadImpl>()->SetOwner(this);
	if (bCreateOwnThread)
	{
		GetImpl<ThreadImpl>()->CreateOwnedThread();
	}
}

Thread::~Thread()
{
	DestroyImpl<PlatformMemoryArena, ThreadImpl>();
}

void Thread::SetName(const String& Name)
{
	GetImpl<ThreadImpl>()->SetName(Name);
}

void Thread::Start(EntryPoint Point)
{
	GetImpl<ThreadImpl>()->Start(Point);
}

void Thread::Abort()
{
	GetImpl<ThreadImpl>()->Abort();
}

void Thread::Join()
{
	GetImpl<ThreadImpl>()->Join();
}

bool Thread::IsRunning()
{
	return GetImpl<ThreadImpl>()->IsRunning();
}

bool Thread::IsAborted()
{
	return GetImpl<ThreadImpl>()->IsAborted();
}

bool Thread::IsEqual(const Thread* other) const
{
	return GetImpl<ThreadImpl>()->IsEqual(other->GetImpl<ThreadImpl>());
}

Thread* Thread::Self()
{
	// This will trip if thread was not created by our code.
	if (g_ThreadSelf == nullptr)
	{
		EnterCriticalSection(&g_nonEngineThreadsCS);

		g_ThreadSelf = LD_ARENA_NEW(PlatformMemoryArena, Thread);
		g_ThreadSelf->GetImpl<ThreadImpl>()->SetOwner(g_ThreadSelf);
		g_ThreadSelf->GetImpl<ThreadImpl>()->SetHandles(
			GetCurrentThread(), 
			GetCurrentThreadId()
		);

		g_nonEngineThreads.Add(g_ThreadSelf);

		LeaveCriticalSection(&g_nonEngineThreadsCS);
	}

	return g_ThreadSelf;
}

Thread* Thread::Main()
{
	return g_ThreadMain;
}

void Thread::EnterMainThread()
{
	InitializeCriticalSection(&g_nonEngineThreadsCS);

	HANDLE handle;
	DuplicateHandle(GetCurrentProcess(),
		GetCurrentThread(),
		GetCurrentProcess(),
		&handle,
		0,
		TRUE,
		DUPLICATE_SAME_ACCESS);


	g_ThreadMain = LD_ARENA_NEW(PlatformMemoryArena, Thread, false);
	g_ThreadMain->GetImpl<ThreadImpl>()->SetOwner(g_ThreadMain);
	g_ThreadMain->GetImpl<ThreadImpl>()->SetHandles(
		GetCurrentThread(),
		GetCurrentThreadId()
	);
	g_ThreadMain->SetName("Main Thread");

	g_ThreadSelf = g_ThreadMain;
}

void Thread::ExitMainThread()
{
	DeleteCriticalSection(&g_nonEngineThreadsCS);

	for (Thread* Other : g_nonEngineThreads)
	{
		LD_ARENA_DELETE(PlatformMemoryArena, Other);
	}

	LD_ARENA_DELETE(PlatformMemoryArena, g_ThreadMain);

	g_ThreadSelf = nullptr;
	g_ThreadMain = nullptr;
}

}; // namespace Ludo