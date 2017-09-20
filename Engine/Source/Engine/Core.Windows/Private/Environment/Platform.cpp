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

#include "Core.Windows/Private/Platform/Helper/Wmi.h"
#include "Core.Windows/Private/Platform/Helper/Errors.h"

using namespace Ludo;
using namespace Ludo::Windows;

// ************************************************************************************************

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	LD_UNUSED_PARAMETER(hInstance);
	LD_UNUSED_PARAMETER(hPrevInstance);
	LD_UNUSED_PARAMETER(pScmdline);
	LD_UNUSED_PARAMETER(iCmdshow);

	return Ludo::LudoEntryPoint();
}

// ************************************************************************************************

int main(int argc, char* argv[])
{
	LD_UNUSED_PARAMETER(argc);
	LD_UNUSED_PARAMETER(argv);

	return Ludo::LudoEntryPoint();
}

// ************************************************************************************************

namespace Ludo {
namespace Environment {

// ************************************************************************************************

/// \brief TODO
struct WindowsDebugLogListener : public LogListener
{
private:
	HANDLE m_ConsoleHandle;
	unsigned int m_LastColor;

	Array<Color> m_ColorTable;

public:
	WindowsDebugLogListener()
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

// ************************************************************************************************

WindowsDebugLogListener gWindowsDebugLogListener;
WmiConnection gWmiConnection;

// ************************************************************************************************

LONG WINAPI WindowsExceptionHandler(struct _EXCEPTION_POINTERS *exceptionInfo)
{
	LD_UNUSED_PARAMETER(exceptionInfo);

	LD_LOG(Platform, Error, "~~~~~~~~~~~~ UNHANDLED EXCEPTION OCCURRED ~~~~~~~~~~~");
	LD_LOG(Platform, Error, "Stack Trace:");

	StackTrace Trace;
	if (StackTrace::Generate(Trace, (void*)exceptionInfo).Failed())
	{
		LD_LOG(Platform, Error, "Failed to generate stack trace.");
		return 0;
	}
	else
	{
		for (int i = 0; i < Trace.FrameCount(); i++)
		{
			StackFrame Frame = Trace.GetFrame(i);
			if (Frame.Resolve().Failed())
			{
				LD_LOGF(Platform, Error, "[%i] 0x%p (Symbols Unresolved)", i, Frame.Address);
			}
			else
			{
				LD_LOGF(Platform, Error, "[%i] (%s:%i) %s", i, Frame.File.Data(), Frame.Line, Frame.Function.Data());
			}
		}
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

// ************************************************************************************************

BOOL WINAPI WindowsConsoleCloseCallback(_In_ DWORD dwCtrlType)
{
	LD_UNUSED_PARAMETER(dwCtrlType);

	LD_LOG(Platform, Warning, "Recieved console close callback. Terminating.");

	Environment::Exit();

	return TRUE;
}

// ************************************************************************************************

void PreInit()
{
	// Register the debug log output as visual studio dosen't show stdio in the output window by default!
	gWindowsDebugLogListener.Register();
}

// ************************************************************************************************

void Init()
{
	// Store main thread information.
	Thread::EnterMainThread();

	// Disable any pita popups that occassionally occur when checking drive file
	// sizes, errors in libraries, etc.	
	LD_LOG(Platform, Info, "Setting error mask to supress non-critical error popups.");
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOALIGNMENTFAULTEXCEPT | SEM_NOOPENFILEERRORBOX | SEM_NOGPFAULTERRORBOX);

	// Symbols need to be initialized if we want debug stack traces and such
	// when the allocators are initialized.
	LD_LOG(Platform, Info, "Attempting to load symbol information if available ...");
	SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
	if (!SymInitialize(GetCurrentProcess(), NULL, TRUE))
	{
		LD_LOG(Platform, Info, "\tFailed to load symbol information.");
	}
	else
	{
		LD_LOG(Platform, Info, "\tSuccessfully loaded symbol information.");
	}

	// Install exception filter.	
	LD_LOG(Platform, Info, "Installing exception handler.");
	SetUnhandledExceptionFilter(WindowsExceptionHandler);

	LD_LOG(Platform, Info, "Installing console ctrl handler.");
	SetConsoleCtrlHandler(WindowsConsoleCloseCallback, true);

	// Seed random.
	LD_LOG(Platform, Info, "Seeding random number information.");
	srand((int)GetTickCount());

	// Setup COM information.
	LD_LOG(Platform, Info, "Initializing COM interface.");
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
	LD_LOG(Platform, Info, "Initializing WMI connection.");
	if (gWmiConnection.Connect().Failed())
	{
		LD_LOG(Platform, Warning, "Failed to create connection to WMI, WMI queries may return invalid data.");
	}

	// Initialize Winsock
	LD_LOG(Platform, Info, "Initializing winsock library.");
	WSADATA winsockData;
	int WasResult = WSAStartup(MAKEWORD(2, 2), &winsockData);
	if (FAILED(WasResult))
	{
		AssertLastSystemError("WSAStartup");
	}
}

// ************************************************************************************************

void Term()
{
	// Cleanup winsock interface.
	LD_LOG(Platform, Info, "Terminating COM interface.");
	WSACleanup();
	
	// Cleanup WMI connection.
	if (gWmiConnection.IsConnected())
	{
		LD_LOG(Platform, Info, "Disconnecting WMI connection.");
		gWmiConnection.Disconnect();
	}

	// Cleanup com library.
	LD_LOG(Platform, Info, "Terminating winsock library.");
	CoUninitialize();

	// Cleanup main thread information.
	Thread::ExitMainThread();
}

// ************************************************************************************************

void DebugBreak()
{
	::DebugBreak();
}

// ************************************************************************************************

void Exit()
{
	abort();
}

// ************************************************************************************************

void Delay(TimeSpan time)
{
	Sleep((int)time.GetTotalMilliseconds());
}

// ************************************************************************************************

}; // namespace Environment
}; // namespace Ludo