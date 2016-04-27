// Copyright 2016 TwinDrills

#include "Core/Debug/Assert.h"
#include "Core/Types/String.h"
#include "Core/Platform/Platform.h"

namespace Ludo {

void AssertFailed(const char* Message, const char* File, int Line)
{
	const char* Format = \
		"----- Asert Failed -----\n" \
		"Condition: {Condition}\n" \
		"File: {File}:{Line}\n";

	StringArgumentList Args;
	Args.Add("Condition", Message);
	Args.Add("File", File);
	Args.Add("Line", Line);

	Log(LogCore, LogError, Format, Args);

	Platform::DebugBreak();
	Platform::Exit();
}

void AssertFailed(const char* Message, const char* File, int Line, const char* Format)
{
	StringArgumentList Args;
	AssertFailed(Message, File, Line, Format, Args);
}

void AssertFailed(const char* Message, const char* File, int Line, const char* Format, StringArgumentList& Args)
{
	const char* RealFormat = \
		"----- Asert Failed -----\n" \
		"Condition: {Condition}\n" \
		"File: {File}:{Line}\n" \
		"Message: {UserMessage}";

	String UserMessage = String::Format(Format, Args);

	StringArgumentList RealArgs;
	RealArgs.Add("Condition", Message);
	RealArgs.Add("File", File);
	RealArgs.Add("Line", Line);
	RealArgs.Add("Message", UserMessage.Data());

	Log(LogCore, LogError, RealFormat, RealArgs);

	Platform::DebugBreak();
	Platform::Exit();
}

}; // namespace Ludo