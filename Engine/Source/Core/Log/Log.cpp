// Copyright 2016 TwinDrills

#include "Core/Debug/Log.h"
#include "Core/Math/Math.h"
#include "Core/Types/String.h"
#include "Core/Types/StringFormatters/SimpleStringArgumentFormatter.h"
#include "Core/Types/StringFormatters/TimeStringArgumentFormatter.h"
#include "Core/Types/Color.h"
#include "Core/Containers/Array.h"
#include "Core/Platform/Time.h"

#include <memory>

namespace Ludo {

Array<LogOutput*> gGlobalLogOutputs;
int gGlobalLogVerbosity = 0;

// Declare all the default log severities.
#define LOG_SEVERITY LogSeverityImplement
#include "Core/Debug/LogSeverities.inc"
#undef LOG_SEVERITY

#define LOG_CATEGORY LogCategoryImplement
#include "Core/Debug/LogCategories.inc"
#undef LOG_CATEGORY

void LogPrintf(const char* Format, ...)
{
	va_list Args;
	va_start(Args, Format);

	char StaticBuffer[512];
	char* Buffer = StaticBuffer;

	int num = vsnprintf(Buffer, 512, Format, Args);
	if (num >= 511)
	{
		Buffer = new char[num + 1];
		vsnprintf(Buffer, num + 1, Format, Args);
	}

	OutputDebugStringA(Buffer);
	printf(Buffer);

	if (Buffer != StaticBuffer)
	{
		delete[] Buffer;
	}

	va_end(Args);
}

LogSeverity::LogSeverity(const char* InName, const int InVerbosityLevel, unsigned int InDisplayColorARGB)
	: VerbosityLevel(InVerbosityLevel)
	, DisplayColorARGB(InDisplayColorARGB)
	, bSuppressed(false)
{
	strncpy(Name, InName, MaxNameLength - 1);
	Name[MaxNameLength - 1] = '\0';
}

LogCategory::LogCategory(const char* InName, const int InVerbosityLevel)
	: VerbosityLevel(InVerbosityLevel)
	, bSuppressed(false)
{
	strncpy(Name, InName, MaxNameLength - 1);
	Name[MaxNameLength - 1] = '\0';
}

void LogOutput::GlobalEmit(const LogSeverity& Severity, const LogCategory& Category, const char* File, int Line, const char* Message)
{
	StringArgumentList Args;
	GlobalEmit(Severity, Category, File, Line, Message, Args);
}

void LogOutput::GlobalEmit(const LogSeverity& Severity, const LogCategory& Category, const char* File, int Line, const char* Message, StringArgumentList& Args)
{
	int Verbosity = Math::Min(Severity.VerbosityLevel, Category.VerbosityLevel);
	if (Verbosity >= gGlobalLogVerbosity)
	{
		return;
	}

	String FormattedMessage = String::Format(Message, Args);

	// Strip off any path provided.
	String LocalPath = File;
	const char* LastBackslash = strrchr(File, '\\');
	const char* LastForeslash = strrchr(File, '/');
	if (LastBackslash != nullptr || LastForeslash != nullptr)
	{
		LocalPath = (LastBackslash > LastForeslash) ? LastBackslash + 1 : LastForeslash + 1;
	}

	String FileInfo = (LocalPath + String(":") + String(Line));

	SimpleStringArgumentFormatter PaddedStringFormatLong;
	PaddedStringFormatLong.PadRight(25, ' ');

	SimpleStringArgumentFormatter PaddedStringFormatSmall;
	PaddedStringFormatSmall.PadRight(10, ' ');

	SimpleStringArgumentFormatter NumberStringFormatSmall;
	NumberStringFormatSmall.SetNumberPrecision(3);
	NumberStringFormatSmall.PadLeft(7, '0');

	StringArgumentList LogFormatArgs;
	LogFormatArgs.Add("Time", Time::AppDuration().GetTotalMilliseconds() / 1000.0f, &NumberStringFormatSmall);
	LogFormatArgs.Add("Category", Category.Name, &PaddedStringFormatSmall);
	LogFormatArgs.Add("Severity", Severity.Name, &PaddedStringFormatSmall);
	LogFormatArgs.Add("FileInfo", FileInfo.Data(), &PaddedStringFormatLong);
	LogFormatArgs.Add("Message", FormattedMessage.Data());

	String FinalFormattedMessage = String::Format("[{Time}] [{Category}] [{Severity}] ({FileInfo}) {Message}\n", LogFormatArgs);
	
	for (auto output : gGlobalLogOutputs)
	{
		output->Emit(FinalFormattedMessage.Data());
	}
}

void LogOutput::SetLogVerbosity(int Verbosity)
{
	gGlobalLogVerbosity = Verbosity;
}

int LogOutput::GetLogVerbosity()
{
	return gGlobalLogVerbosity;
}

void LogOutput::Register()
{
	gGlobalLogOutputs.AddUnique(this);
}

void LogOutput::Unregister()
{
	gGlobalLogOutputs.Remove(this);
}

}; // namespace Ludo