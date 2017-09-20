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

#include "Core/Public/Debug/Assert.h"
#include "Core/Public/Types/String.h"
#include "Core/Public/Environment/Environment.h"

namespace Ludo {
    
// ************************************************************************************************

void AssertFailed(const char* Message, const char* File, int Line)
{
	const char* Format = \
        "\n" \
		"----- Assert Failed -----\n" \
		"Condition: {Condition}\n" \
		"File: {File}:{Line}\n" \
        "\n";

	StringArgumentList Args;
	Args.Add("Condition", Message);
	Args.Add("File", File);
	Args.Add("Line", Line);

	LD_LOGF(Core, Error, Format, Args);

	Environment::DebugBreak();
	Environment::Exit();
}

// ************************************************************************************************

void AssertFailed(const char* Message, const char* File, int Line, const char* Format)
{
	StringArgumentList Args;
	AssertFailedF(Message, File, Line, Format, Args);
}

// ************************************************************************************************

void AssertFailedF(const char* Message, const char* File, int Line, const char* Format, StringArgumentList& Args)
{
	const char* RealFormat = \
        "\n" \
		"----- Assert Failed -----\n" \
		"Condition: {Condition}\n" \
		"File: {File}:{Line}\n" \
		"Message: {Message}\n" \
        "\n";

	String UserMessage = String::FormatArgs(Format, Args);

	StringArgumentList RealArgs;
	RealArgs.Add("Condition", Message);
	RealArgs.Add("File", File);
	RealArgs.Add("Line", Line);
	RealArgs.Add("Message", UserMessage.Data());

	LD_LOGF(Core, Error, RealFormat, RealArgs);

	Environment::DebugBreak();
	Environment::Exit();
}

// ************************************************************************************************

void AssertFailedF(const char* Message, const char* File, int Line, const char* Format, ...)
{
	const char* RealFormat = \
        "\n" \
		"----- Assert Failed -----\n" \
		"Condition: {Condition}\n" \
		"File: {File}:{Line}\n" \
		"Message: {Message}\n" \
        "\n";

	va_list list;
	va_start(list, Format);
	String UserMessage = String::FormatVA(Format, list);
	va_end(list);

	StringArgumentList RealArgs;
	RealArgs.Add("Condition", Message);
	RealArgs.Add("File", File);
	RealArgs.Add("Line", Line);
	RealArgs.Add("Message", UserMessage.Data());

	LD_LOGF(Core, Error, RealFormat, RealArgs);

	Environment::DebugBreak();
	Environment::Exit();
}

// ************************************************************************************************

}; // namespace Ludo