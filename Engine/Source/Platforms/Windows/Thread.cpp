// Copyright 2016 TwinDrills

#include "Core/Platform/Thread.h"
#include "Core/Memory/Allocator.h"
#include "Platforms/Windows/Helper/Errors.h"

#include <Windows.h>

using namespace Ludo;
using namespace Ludo::Windows;

namespace Ludo {

__declspec(thread) Thread* g_ThreadSelf = NULL;
				   Thread* g_ThreadMain = NULL;

class Thread::Impl
{
private:
	Thread::EntryPoint m_EntryPoint;
	HANDLE m_Thread;
	DWORD m_ThreadId;
	String m_Name;
	bool m_Finished;
	bool m_Aborted;
	Thread* m_Base;

public:
	Impl(String Name, EntryPoint Point, Thread Base)
		: m_EntryPoint(nullptr)
		, m_Thread(INVALID_HANDLE_VALUE)
		, m_ThreadId(0)
		, m_Name("")
		, m_Finished(false)
		, m_Aborted(false)
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
	}

	~Impl()
	{
		Join();

		CloseHandle(m_Thread);
		m_Thread = nullptr;
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
		Impl* ThreadImpl = reinterpret_cast<Impl*>(lpThreadParameter);

		g_ThreadSelf = ThreadImpl->m_Base;

		// Set thread name.
		if (ThreadImpl->m_ThreadId != 0 &&
			!ThreadImpl->m_Name.IsEmpty())
		{
			SetVsThreadName(ThreadImpl->m_ThreadId, ThreadImpl->m_Name.Data());
		}

		// Call entry point.
		ThreadImpl->m_EntryPoint(ThreadImpl->m_Base);
		ThreadImpl->m_Finished = true;

		g_ThreadSelf = nullptr;

		return 0;
	}

	void Start()
	{
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
};
 
void InitMainThread()
{
	DWORD id = GetCurrentThreadId();

	HANDLE handle;
	DuplicateHandle(GetCurrentProcess(),
		GetCurrentThread(),
		GetCurrentProcess(),
		&handle,
		0,
		TRUE,
		DUPLICATE_SAME_ACCESS);

	g_ThreadSelf = new Thread();
	g_ThreadMain = g_ThreadSelf;

	g_ThreadSelf->Set_Name("Main Thread");
}

void TermMainThread()
{
	ArenaAllocator<PlatformMemoryArena> Allocator;
	Allocator.Delete<Thread>(g_ThreadSelf);

	g_ThreadSelf = nullptr;
	g_ThreadMain = nullptr;
}

// todo: Just have Create function thats overriden per platform? But what about delete?
//		 who deals with the allocation of the thread type?

Thread::Thread(String Name, EntryPoint Point)
{
	ArenaAllocator<PlatformMemoryArena> Allocator;

	m_Impl = Allocator.New<Impl>(Name, Point, this);
	Assert(m_Impl != nullptr);
}

Thread::~Thread()
{
	ArenaAllocator<PlatformMemoryArena> Allocator;

	Allocator.Delete<Impl>(m_Impl);
	m_Impl = nullptr;
}

void Thread::Start()
{
	m_Impl->Start();
}

void Thread::Abort()
{
	m_Impl->Abort();
}

void Thread::Join()
{
	m_Impl->Join();
}

bool Thread::IsRunning()
{
	return m_Impl->IsRunning();
}

bool Thread::IsAborted()
{
	return m_Impl->IsAborted();
}

Thread* Thread::Self()
{
	return g_ThreadSelf;
}

Thread* Thread::Main()
{
	return g_ThreadMain;
}

}; // namespace Ludo