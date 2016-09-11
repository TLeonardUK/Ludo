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

	LogF(LogPlatform, LogError, "{FunctionName} failed with system error {ErrorNumber} ({ErrorDescription}).", Args);
}

void AssertLastSystemError(String SourceFunction)
{
	::DWORD Error = GetLastError();

	StringArgumentList Args;
	Args.Add("ErrorNumber", (int)Error);
	Args.Add("ErrorDescription", FormatSystemError(Error));
	Args.Add("FunctionName", SourceFunction);

	LogF(LogPlatform, LogError, "{FunctionName} failed with system error {ErrorNumber} ({ErrorDescription}).", Args);
	ConstantAssertMsgF("{FunctionName} failed with system error {ErrorNumber} ({ErrorDescription}).", Args);
}

}; // namespace Windows
}; // namespace Ludo