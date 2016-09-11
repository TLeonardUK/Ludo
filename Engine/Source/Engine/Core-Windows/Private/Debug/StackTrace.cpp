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

struct DbgHelpGlobals
{
	CRITICAL_SECTION Mutex;

	DbgHelpGlobals()
	{
		InitializeCriticalSection(&Mutex);
	}

	~DbgHelpGlobals()
	{
		DeleteCriticalSection(&Mutex);

	}
} gDbgHelpGlobals;

Error StackFrame::Resolve()
{
	ErrorType Result = ErrorType::Success;

	DWORD64 Displacement = 0;
	const int MaxSymbolLength = 1024;
	char Buffer[sizeof(SYMBOL_INFO) + MaxSymbolLength * sizeof(TCHAR)];

	PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)Buffer;
	pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	pSymbol->MaxNameLen = MaxSymbolLength;

	EnterCriticalSection(&gDbgHelpGlobals.Mutex);

	char FunctionBuffer[MaxSymbolLength];
	char FilenameBuffer[MaxSymbolLength];

	memset(FunctionBuffer, 0, MaxSymbolLength);
	memset(FilenameBuffer, 0, MaxSymbolLength);

	if (SymFromAddr(GetCurrentProcess(), Address, &Displacement, pSymbol) == TRUE)
	{
		strcpy_s(FunctionBuffer, MaxSymbolLength, pSymbol->Name);
		Function = FunctionBuffer;

		IMAGEHLP_LINE64 LineInfo;
		LineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

		DWORD LineDisplacement;
		if (SymGetLineFromAddr64(GetCurrentProcess(), Address, &LineDisplacement, &LineInfo))
		{
			strcpy_s(FilenameBuffer, MaxSymbolLength, LineInfo.FileName);
			Line = LineInfo.LineNumber;
			File = FilenameBuffer;
		}
		else
		{
		//	LogLastSystemError("SymGetLineFromAddr64");
			Line = 0;
			File = "";
			Result = ErrorType::Failure;
		}
	}
	else
	{
		LogLastSystemError("SymFromAddr");
		Result = ErrorType::Failure;
	}

	LeaveCriticalSection(&gDbgHelpGlobals.Mutex);

	return Result;
}

Error StackTrace::Generate(StackTrace& Trace, ExceptionRecord Record)
{
	const int MaxFrames = 512; // Maxium frames to try and capture.

	Trace.m_Frames.Empty();

	if (Record != NULL)
	{
		// If thread is not given, use current.
		HANDLE CurrentThread = GetCurrentThread();
		struct _EXCEPTION_POINTERS* Exceptioninfo = reinterpret_cast<struct _EXCEPTION_POINTERS *>(Record);

		// Capture context for this thread.
		CONTEXT Context;
		ZeroMemory(&Context, sizeof(CONTEXT));

		// 32bit has no function call to get the current
		// threads context, so its asm trickery time :3
#ifdef LD_ARCHITECTURE_X86

		if (Exceptioninfo != NULL)
		{
			memcpy(&Context, Exceptioninfo->ContextRecord, sizeof(CONTEXT));
		}
		else
		{
			Context.ContextFlags = CONTEXT_CONTROL;
			__asm
			{
			Label:
				mov[Context.Ebp], ebp;
				mov[Context.Esp], esp;
				mov eax, [Label];
				mov[Context.Eip], eax;
			}
		}

		// 64bit does though, w00t.
#else
		if (Exceptioninfo != NULL)
		{
			memcpy(&Context, Exceptioninfo->ContextRecord, sizeof(CONTEXT));
		}
		else
		{
			RtlCaptureContext(&Context);
		}

#endif

		// Build the initial stack frame.
		STACKFRAME64 StackFrame64;
		DWORD		 MachineType;
		ZeroMemory(&StackFrame64, sizeof(STACKFRAME64));

#if defined(LD_ARCHITECTURE_X86)
		MachineType = IMAGE_FILE_MACHINE_I386;
		StackFrame64.AddrPC.Offset = Context.Eip;
		StackFrame64.AddrPC.Mode = AddrModeFlat;
		StackFrame64.AddrFrame.Offset = Context.Ebp;
		StackFrame64.AddrFrame.Mode = AddrModeFlat;
		StackFrame64.AddrStack.Offset = Context.Esp;
		StackFrame64.AddrStack.Mode = AddrModeFlat;
#elif defined(LD_ARCHITECTURE_X64)
		MachineType = IMAGE_FILE_MACHINE_AMD64;
		StackFrame64.AddrPC.Offset = Context.Rip;
		StackFrame64.AddrPC.Mode = AddrModeFlat;
		StackFrame64.AddrFrame.Offset = Context.Rsp;
		StackFrame64.AddrFrame.Mode = AddrModeFlat;
		StackFrame64.AddrStack.Offset = Context.Rsp;
		StackFrame64.AddrStack.Mode = AddrModeFlat;
#else
		Log(LogPlatform, LogError, "Platform does not support stack walking.");
		return ErrorType::NotSupported;
#endif

		EnterCriticalSection(&gDbgHelpGlobals.Mutex);

		// Stack trace!
		while (Trace.m_Frames.Length() < MaxFrames)
		{
			if (!StackWalk64(MachineType, GetCurrentProcess(), CurrentThread, &StackFrame64, &Context,
				NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
			{
				break;
			}

			if (StackFrame64.AddrPC.Offset != 0)
			{
				StackFrame Frame;
				Frame.IsResolved = false;
#if defined(LD_ARCHITECTURE_X86)
				Frame.Address = (intptr)StackFrame64.AddrPC.Offset;
#elif defined(LD_ARCHITECTURE_X64)
				Frame.Address = (intptr)StackFrame64.AddrPC.Offset;
#else
				Log(LogPlatform, LogError, "Platform does not support stack walking.");
				return ErrorType::NotSupported;
#endif
				Trace.m_Frames.Push(Frame);
			}
			else
			{
				// Reached the base of the stack.
				break;
			}
		}

		LeaveCriticalSection(&gDbgHelpGlobals.Mutex);
	}
	else
	{
		void* FramePointers[MaxFrames];
		int FrameCount = CaptureStackBackTrace(0, MaxFrames, FramePointers, NULL);
		for (int i = 0; i < FrameCount; i++)
		{
			StackFrame Frame;
			Frame.Address = (intptr)FramePointers[i];
			Frame.IsResolved = false;
			Trace.m_Frames.Push(Frame);
		}
	}

	return ErrorType::Success;
}

}; // namespace Ludo