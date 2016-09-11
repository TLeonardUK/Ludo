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

#include "Core-Windows/Private/Platform/Helper/Wmi.h"
#include "Core-Windows/Private/Platform/Helper/Errors.h"

using namespace Ludo;
using namespace Ludo::Windows;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	UNUSED_PARAMETER(hInstance);
	UNUSED_PARAMETER(hPrevInstance);
	UNUSED_PARAMETER(pScmdline);
	UNUSED_PARAMETER(iCmdshow);

	return Ludo::LudoEntryPoint();
}

int main(int argc, char* argv[])
{
	UNUSED_PARAMETER(argc);
	UNUSED_PARAMETER(argv);

	return Ludo::LudoEntryPoint();
} 

namespace Ludo {
namespace Environment {

/// \brief TODO
struct WindowsDebugLogOutput : public LogOutput
{
private:
	HANDLE m_ConsoleHandle;
	unsigned int m_LastColor;

	Array<Color> m_ColorTable;

public:
	WindowsDebugLogOutput()
	{
		m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		m_LastColor = 0;
	}

private:
	int ColorTableLookup(unsigned int ColorArgb)
	{
		if (m_ColorTable.IsEmpty())
		{
			CONSOLE_SCREEN_BUFFER_INFOEX ConsoleInfo;
			ConsoleInfo.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
			GetConsoleScreenBufferInfoEx(m_ConsoleHandle, &ConsoleInfo);

			for (int i = 0; i < 16; i++)
			{
				m_ColorTable.Add(Color::FromAbgr(ConsoleInfo.ColorTable[i]));
			}
		}

		Color TargetColor(ColorArgb);
		return TargetColor.FindClosestPaletteIndex(m_ColorTable);
	}

	void ChangeColor(unsigned int ColorArgb)
	{
		SetConsoleTextAttribute(m_ConsoleHandle, (WORD)ColorTableLookup(ColorArgb));
		m_LastColor = ColorArgb;
	}

	virtual void Emit(const char* Message, const unsigned int ColorArgb) override
	{
		if (m_ConsoleHandle != INVALID_HANDLE_VALUE && m_LastColor != ColorArgb)
		{
			ChangeColor(ColorArgb);
		}
		printf(Message);
		OutputDebugStringA(Message);
	}
};

WindowsDebugLogOutput gWindowsDebugLogOutput;
WmiConnection gWmiConnection;

LONG WINAPI WindowsExceptionHandler(struct _EXCEPTION_POINTERS *exceptionInfo)
{
	UNUSED_PARAMETER(exceptionInfo);

	Log(LogPlatform, LogError, "~~~~~~~~~~~~ UNHANDLED EXCEPTION OCCURRED ~~~~~~~~~~~");
	Log(LogPlatform, LogError, "Stack Trace:");

	StackTrace Trace;
	if (StackTrace::Generate(Trace, (void*)exceptionInfo).Failed())
	{
		Log(LogPlatform, LogError, "Failed to generate stack trace.");
		return 0;
	}
	else
	{
		for (int i = 0; i < Trace.FrameCount(); i++)
		{
			StackFrame Frame = Trace.GetFrame(i);
			if (Frame.Resolve().Failed())
			{
				LogF(LogPlatform, LogError, "[%i] 0x%p (Symbols Unresolved)", i, Frame.Address);
			}
			else
			{
				LogF(LogPlatform, LogError, "[%i] (%s:%i) %s", i, Frame.File.Data(), Frame.Line, Frame.Function.Data());
			}
		}
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

BOOL WINAPI WindowsConsoleCloseCallback(_In_ DWORD dwCtrlType)
{
	UNUSED_PARAMETER(dwCtrlType);

	Log(LogPlatform, LogWarning, "Recieved console close callback. Terminating.");

	Environment::Exit();

	return TRUE;
}

void PreInit()
{
	// Register the debug log output as visual studio dosen't show stdio in the output window by default!
	gWindowsDebugLogOutput.Register();
}

void Init()
{
	// Store main thread information.
	Thread::EnterMainThread();

	// Disable any pita popups that occassionally occur when checking drive file
	// sizes, errors in libraries, etc.	
	Log(LogPlatform, LogInfo, "Setting error mask to supress non-critical error popups.");
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOALIGNMENTFAULTEXCEPT | SEM_NOOPENFILEERRORBOX | SEM_NOGPFAULTERRORBOX);

	// Symbols need to be initialized if we want debug stack traces and such
	// when the allocators are initialized.
	Log(LogPlatform, LogInfo, "Attempting to load symbol information if available ...");
	SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
	if (!SymInitialize(GetCurrentProcess(), NULL, TRUE))
	{
		Log(LogPlatform, LogInfo, "\tFailed to load symbol information.");
	}
	else
	{
		Log(LogPlatform, LogInfo, "\tSuccessfully loaded symbol information.");
	}

	// Install exception filter.	
	Log(LogPlatform, LogInfo, "Installing exception handler.");
	SetUnhandledExceptionFilter(WindowsExceptionHandler);

	Log(LogPlatform, LogInfo, "Installing console ctrl handler.");
	SetConsoleCtrlHandler(WindowsConsoleCloseCallback, true);

	// Seed random.
	Log(LogPlatform, LogInfo, "Seeding random number information.");
	srand((int)GetTickCount());

	// Setup COM information.
	Log(LogPlatform, LogInfo, "Initializing COM interface.");
	HRESULT ComResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(ComResult))
	{
		AssertLastSystemError("CoInitializeEx");
	}

	// Setup COM security levels.
	ComResult = CoInitializeSecurity(
		NULL,
		-1,                          // COM authentication
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
	);
	if (FAILED(ComResult))
	{
		AssertLastSystemError("CoInitializeSecurity");
	}

	// Initialize WMI connection.
	Log(LogPlatform, LogInfo, "Initializing WMI connection.");
	if (gWmiConnection.Connect().Failed())
	{
		Log(LogPlatform, LogWarning, "Failed to create connection to WMI, WMI queries may return invalid data.");
	}

	// Initialize Winsock
	Log(LogPlatform, LogInfo, "Initializing winsock library.");
	WSADATA winsockData;
	int WasResult = WSAStartup(MAKEWORD(2, 2), &winsockData);
	if (FAILED(WasResult))
	{
		AssertLastSystemError("WSAStartup");
	}
}

void Term()
{
	// Cleanup winsock interface.
	Log(LogPlatform, LogInfo, "Terminating COM interface.");
	WSACleanup();
	
	// Cleanup WMI connection.
	if (gWmiConnection.IsConnected())
	{
		Log(LogPlatform, LogInfo, "Disconnecting WMI connection.");
		gWmiConnection.Disconnect();
	}

	// Cleanup com library.
	Log(LogPlatform, LogInfo, "Terminating winsock library.");
	CoUninitialize();

	// Cleanup main thread information.
	Thread::ExitMainThread();
}

void DebugBreak()
{
	::DebugBreak();
}

void Exit()
{
	abort();
}

void Delay(TimeSpan time)
{
	Sleep((int)time.GetTotalMilliseconds());
}

}; // namespace Environment
}; // namespace Ludo