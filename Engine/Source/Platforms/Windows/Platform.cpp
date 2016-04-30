// Copyright 2016 TwinDrills

#include "Core/Platform/Platform.h"
#include "Platforms/Windows/Helper/Errors.h"

#include <cstdlib>
#include <Windows.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <stdarg.h>
#include <cstdio>

#pragma warning(push)
#pragma warning(disable:4091) // 2015 introduced some extra warnings about typedef's, dbghelp.h/shlobj.h now spams them :(
#include <dbghelp.h>
#include <Shlobj.h>
#pragma warning(pop)

using namespace Ludo;
using namespace Ludo::Windows;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	UNUSED_PARAMETER(hInstance);
	UNUSED_PARAMETER(hPrevInstance);
	UNUSED_PARAMETER(pScmdline);
	UNUSED_PARAMETER(iCmdshow);

	Ludo::Error result = Ludo::LudoEntryPoint();
	if (result.Failed())
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int main(int argc, char* argv[])
{
	UNUSED_PARAMETER(argc);
	UNUSED_PARAMETER(argv);

	Ludo::Error result = Ludo::LudoEntryPoint();
	if (result.Failed())
	{
		return 1;
	}
	else
	{
		return 0;
	}
} 

namespace Ludo {

void InitMainThread();
void TermMainThread();

namespace Platform {

/// \brief TODO
struct WindowsDebugLogOutput : public LogOutput
{
private:
	virtual void Emit(const char* Message) override
	{
		OutputDebugStringA(Message);
	}
};

WindowsDebugLogOutput gWindowsDebugLogOutput;

LONG WINAPI WindowsExceptionHandler(struct _EXCEPTION_POINTERS *exceptionInfo)
{
	UNUSED_PARAMETER(exceptionInfo);

	Log(LogPlatform, LogError, "~~~~~~~~~~~~ UNHANDLED EXCEPTION OCCURRED ~~~~~~~~~~~");
	Log(LogPlatform, LogError, "Stack Trace:");
/*
	StackTrace Trace = StackTrace::Generate();
	for (int i = 0; i < Trace.FrameCount(); i++)
	{
		StackTrace Trace = Trace.GetFrame(i);
		Log(LogPlatform, LogError, "[{Index}] ({File}:{Line}) {Function}");
	}
*/
	return EXCEPTION_EXECUTE_HANDLER;
}

BOOL WINAPI WindowsConsoleCloseCallback(_In_ DWORD dwCtrlType)
{
	UNUSED_PARAMETER(dwCtrlType);

	Log(LogPlatform, LogWarning, "Recieved console close callback. Terminating.");

	Platform::Exit();

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
	InitMainThread();

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
	// Cleanup winsock library.
	Log(LogPlatform, LogInfo, "Terminating winsock library.");
	CoUninitialize();

	// Cleanup COM interface.
	Log(LogPlatform, LogInfo, "Terminating COM interface.");
	WSACleanup();

	// Cleanup main thread information.
	TermMainThread();
}

void DebugBreak()
{
	::DebugBreak();
}

void Exit()
{
	abort();
}

}; // namespace Platform
}; // namespace Ludo