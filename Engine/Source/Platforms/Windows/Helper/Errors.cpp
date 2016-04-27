// Copyright 2016 TwinDrills

#include "Platforms/Windows/Helper/Errors.h"
#include "Core/Types/String.h"

namespace Ludo {
namespace Windows {

String FormatSystemError(::DWORD Error)
{
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		Error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	String result = (const char*)lpMsgBuf;

	LocalFree(lpMsgBuf);

	return result;
}

void LogLastSystemError(String SourceFunction)
{
	::DWORD Error = GetLastError();

	StringArgumentList Args;
	Args.Add("ErrorNumber", (int)Error);
	Args.Add("ErrorDescription", FormatSystemError(Error));
	Args.Add("FunctionName", SourceFunction);

	Log(LogPlatform, LogError, "{FunctionName} failed with system error {ErrorNumber} ({ErrorDescription}).", Args);
}

void AssertLastSystemError(String SourceFunction)
{
	::DWORD Error = GetLastError();

	StringArgumentList Args;
	Args.Add("ErrorNumber", (int)Error);
	Args.Add("ErrorDescription", FormatSystemError(Error));
	Args.Add("FunctionName", SourceFunction);

	Log(LogPlatform, LogError, "{FunctionName} failed with system error {ErrorNumber} ({ErrorDescription}).", Args);
	ConstantAssertMsg("{FunctionName} failed with system error {ErrorNumber} ({ErrorDescription}).", Args);
}

}; // namespace Windows
}; // namespace Ludo