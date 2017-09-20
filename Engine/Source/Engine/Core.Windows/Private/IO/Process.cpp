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

class ProcessImpl
{
private:
	PROCESS_INFORMATION m_ProcessInfo;
	STARTUPINFOA		m_StartupInfo;
	SECURITY_ATTRIBUTES	m_SecurityAttributes;

	HANDLE				m_StdInWrite;
	HANDLE				m_StdInRead;
	HANDLE				m_StdOutWrite;
	HANDLE				m_StdOutRead;

	bool				m_bRedirectStdInOut;
	bool				m_Attached;

public:
	ProcessImpl()
		: m_Attached(false)
		, m_StdInRead(INVALID_HANDLE_VALUE)
		, m_StdInWrite(INVALID_HANDLE_VALUE)
		, m_StdOutRead(INVALID_HANDLE_VALUE)
		, m_StdOutWrite(INVALID_HANDLE_VALUE)
	{
		ZeroMemory(&m_ProcessInfo, sizeof(PROCESS_INFORMATION));
		ZeroMemory(&m_StartupInfo, sizeof(STARTUPINFOA));
		ZeroMemory(&m_SecurityAttributes, sizeof(SECURITY_ATTRIBUTES));
	}

	~ProcessImpl()
	{
		if (IsAttached())
		{
			Detach();
		}
	}

	// Worthwhile reading before changing this:
	// https://msdn.microsoft.com/en-gb/library/windows/desktop/ms682499(v=vs.85).aspx

	Error Open(const Path& Command, const Path& WorkingDirectory, const Array<String>& Arguments, bool bRedirectStdInOut) 
	{
		LD_ASSERT(!IsAttached());

		// Escape and pack arguments into a single command line incase it has spaces!
		String CommandLine = Command.ToString().DoubleQuoted();

		for (const String& Arg : Arguments)
		{
			if (Arg.IndexOf(" ") != Arg.EndCharIterator())
			{
				CommandLine += Arg.DoubleQuoted();
			}
			else
			{
				CommandLine += Arg;
			}
			CommandLine += " ";
		}

		if (bRedirectStdInOut)
		{
			// Setup security attributes.
			ZeroMemory(&m_SecurityAttributes, sizeof(SECURITY_ATTRIBUTES));
			m_SecurityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
			m_SecurityAttributes.bInheritHandle = TRUE;
			m_SecurityAttributes.lpSecurityDescriptor = NULL;

			// Create pipes to redirect the stdin/stdout and ensure our side of the pipes 
			// (read for stdout, write for stdin) are not inheritable.
			BOOL Result = CreatePipe(&m_StdOutRead, &m_StdOutWrite, &m_SecurityAttributes, 0);
			if (!Result)
			{
				LogLastSystemError("CreateProcess");
				return ErrorType::Failure;
			}
		
			Result = SetHandleInformation(&m_StdOutRead, HANDLE_FLAG_INHERIT, 0);
			if (!Result)
			{
				LogLastSystemError("SetHandleInformation");
				return ErrorType::Failure;
			}

			Result = CreatePipe(&m_StdInRead, &m_StdInWrite, &m_SecurityAttributes, 0);
			if (!Result)
			{
				LogLastSystemError("CreateProcess");
				return ErrorType::Failure;
			}

			Result = SetHandleInformation(&m_StdInWrite, HANDLE_FLAG_INHERIT, 0);
			if (!Result)
			{
				LogLastSystemError("SetHandleInformation");
				return ErrorType::Failure;
			}		
		}
		else
		{
			m_StdInRead = INVALID_HANDLE_VALUE;
			m_StdInWrite = INVALID_HANDLE_VALUE;
			m_StdOutRead = INVALID_HANDLE_VALUE;
			m_StdOutWrite = INVALID_HANDLE_VALUE;
		}


		// Setup process information.
		ZeroMemory(&m_ProcessInfo, sizeof(PROCESS_INFORMATION));

		// Setup startup info.
		ZeroMemory(&m_StartupInfo, sizeof(STARTUPINFOA));
		m_StartupInfo.cb = sizeof(STARTUPINFOA);

		if (bRedirectStdInOut)
		{
			m_StartupInfo.hStdError = m_StdOutWrite;
			m_StartupInfo.hStdOutput = m_StdOutWrite;
			m_StartupInfo.hStdInput = m_StdInRead;
			m_StartupInfo.dwFlags |= STARTF_USESTDHANDLES;
		}

		// Store state.
		m_Attached = true;
		m_bRedirectStdInOut = bRedirectStdInOut;

		// Time to create the process at last!
		BOOL Result = CreateProcessA(
			nullptr,												// lpApplicationName
			(LPSTR)CommandLine.Data(),								// lpCommandLine
			nullptr,												// lpProcessAttributes
			nullptr,												// lpThreadAttributes
			true,													// bInheritHandles
			0,														// dwCreationFlags
			nullptr,												// lpEnvironment
			(LPCSTR)WorkingDirectory.ToString().Data(),				// lpCurrentDirectory
			&m_StartupInfo,											// lpStartupInfo
			&m_ProcessInfo											// lpProcessInformation
		);
		if (!Result)
		{
			Detach();
			LogLastSystemError("CreateProcess");
			return ErrorType::Failure;
		}

		return ErrorType::Success;
	}

	void Detach() 
	{
		LD_ASSERT(IsAttached());

		// dispose of everything.
		if (m_StdInWrite != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_StdInWrite);
			m_StdInWrite = INVALID_HANDLE_VALUE;
		}
		if (m_StdInRead != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_StdInRead);
			m_StdInRead = INVALID_HANDLE_VALUE;
		}
		if (m_StdOutWrite != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_StdOutWrite);
			m_StdOutWrite = INVALID_HANDLE_VALUE;
		}
		if (m_StdOutRead != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_StdOutRead);
			m_StdOutRead = INVALID_HANDLE_VALUE;
		}
		if (m_ProcessInfo.hProcess != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_ProcessInfo.hProcess);
			m_ProcessInfo.hProcess = INVALID_HANDLE_VALUE;
		}
		if (m_ProcessInfo.hThread != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_ProcessInfo.hThread);
			m_ProcessInfo.hThread = INVALID_HANDLE_VALUE;
		}

		m_Attached = false;
	}

	void Terminate() 
	{
		LD_ASSERT(IsAttached());
		
		TerminateProcess(m_ProcessInfo.hProcess, 1);
	}

	bool IsRunning() 
	{
		LD_ASSERT(IsAttached());

		DWORD ExitCode = 0;

		BOOL Result = GetExitCodeProcess(m_ProcessInfo.hProcess, &ExitCode);
		if (!Result)
		{
			LogLastSystemError("GetExitCodeProcess");
		}

		return (ExitCode == STILL_ACTIVE);
	}

	bool IsAttached() 
	{
		return m_Attached;
	}

	bool Wait(TimeSpan Timeout = TimeSpan::Infinite) 
	{
		LD_ASSERT(IsAttached());

		DWORD TimeoutMs = (DWORD)Timeout.GetTotalMilliseconds();
		if (Timeout == TimeSpan::Infinite)
		{
			TimeoutMs = INFINITE;
		}

		DWORD Status = WaitForSingleObject(m_ProcessInfo.hProcess, TimeoutMs);
		if (Status == WAIT_TIMEOUT)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	int GetExitCode() 
	{
		LD_ASSERT(IsAttached());

		DWORD ExitCode = 0;

		BOOL Result = GetExitCodeProcess(m_ProcessInfo.hProcess, &ExitCode);
		if (!Result)
		{
			LogLastSystemError("GetExitCodeProcess");
		}

		LD_ASSERT(ExitCode != STILL_ACTIVE);

		return ExitCode;
	}

	Error Write(void* Buffer, uint64 BufferLength) 
	{
		LD_ASSERT(IsAttached());
		LD_ASSERT(m_bRedirectStdInOut);

		DWORD BytesOutput = 0;
		BOOL Result = WriteFile(m_StdInWrite, Buffer, (DWORD)BufferLength, &BytesOutput, nullptr);
		if (!Result || BytesOutput != BufferLength)
		{
			LogLastSystemError("WriteFile");
			return ErrorType::Failure;
		}

		return ErrorType::Success;
	}

	Error Read(void* Buffer, uint64 BufferLength) 
	{
		LD_ASSERT(IsAttached());
		LD_ASSERT(m_bRedirectStdInOut);

		DWORD BytesOutput = 0;
		BOOL Result = ReadFile(m_StdOutRead, Buffer, (DWORD)BufferLength, &BytesOutput, nullptr);
		if (!Result || BytesOutput != BufferLength)
		{
			LogLastSystemError("ReadFile");
			return ErrorType::Failure;
		}

		return ErrorType::Success;
	}

	bool AtEnd() 
	{
		// No bytes left and process no longer running, sounds like we're done to me.
		return BytesLeft() <= 0 && !IsRunning();
	}

	void Flush() 
	{
		LD_ASSERT(IsAttached());
		LD_ASSERT(m_bRedirectStdInOut);

		FlushFileBuffers(m_StdInWrite);
	}

	uint64 BytesLeft() 
	{
		DWORD BytesAvailable = 0;
		BOOL Result = PeekNamedPipe(m_StdOutRead, nullptr, 0, nullptr, &BytesAvailable, nullptr);
		if (!Result)
		{
			LogLastSystemError("PeekNamedPipe");
			return 0;
		}
		return BytesAvailable;
	}
};

// ************************************************************************************************

Process::Process()
{
	CreateImpl<PlatformMemoryArena, ProcessImpl>();
}

// ************************************************************************************************

Process::~Process()
{
	DestroyImpl<PlatformMemoryArena, ProcessImpl>();
}

// ************************************************************************************************

Error Process::Open(const Path& Command, const Path& WorkingDirectory, const Array<String>& Arguments, bool bRedirectStdInOut)
{
	return GetImpl<ProcessImpl>()->Open(Command, WorkingDirectory, Arguments, bRedirectStdInOut);
}

// ************************************************************************************************

void Process::Detach()
{
	GetImpl<ProcessImpl>()->Detach();
}

// ************************************************************************************************

void Process::Terminate()
{
	GetImpl<ProcessImpl>()->Terminate();
}

// ************************************************************************************************

bool Process::IsRunning()
{
	return GetImpl<ProcessImpl>()->IsRunning();
}

// ************************************************************************************************

bool Process::IsAttached()
{
	return GetImpl<ProcessImpl>()->IsAttached();
}

// ************************************************************************************************

bool Process::Wait(TimeSpan Timeout)
{
	return GetImpl<ProcessImpl>()->Wait(Timeout);
}

// ************************************************************************************************

int Process::GetExitCode()
{
	return GetImpl<ProcessImpl>()->GetExitCode();
}

// ************************************************************************************************

Error Process::Write(void* Buffer, uint64 BufferLength)
{
	return GetImpl<ProcessImpl>()->Write(Buffer, BufferLength);
}

// ************************************************************************************************

Error Process::Read(void* Buffer, uint64 BufferLength)
{
	return GetImpl<ProcessImpl>()->Read(Buffer, BufferLength);
}

// ************************************************************************************************

bool Process::AtEnd()
{
	return GetImpl<ProcessImpl>()->AtEnd();
}

// ************************************************************************************************

void Process::Flush()
{
	GetImpl<ProcessImpl>()->Flush();
}

// ************************************************************************************************

uint64 Process::BytesLeft()
{
	return GetImpl<ProcessImpl>()->BytesLeft();
}

// ************************************************************************************************

}; // namespace Ludo