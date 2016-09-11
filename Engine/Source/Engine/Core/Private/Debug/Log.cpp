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

#include "Core/Public/Debug/Log.h"
#include "Core/Public/Types/String.h"
#include "Core/Public/Types/Formatters/SimpleStringFormatter.h"
#include "Core/Public/Types/Formatters/TimeStringFormatter.h"
#include "Core/Public/Types/Color.h"
#include "Core/Public/Types/Array.h"
#include "Core/Public/Time/Time.h"

#include <memory>

namespace Ludo {

Array<LogOutput*> gGlobalLogOutputs;
int gGlobalLogVerbosity = 0;

// Declare all the default log severities.
#define LOG_SEVERITY LogSeverityImplement
#include "Core/Private/Debug/LogSeverities.inc"
#undef LOG_SEVERITY

#define LOG_CATEGORY LogCategoryImplement
#include "Core/Private/Debug/LogCategories.inc"
#undef LOG_CATEGORY

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
	GlobalEmitF(Severity, Category, File, Line, Message, Args);
}

void LogOutput::GlobalEmitF(const LogSeverity& Severity, const LogCategory& Category, const char* File, int Line, const char* Message, StringArgumentList& Args)
{
	int Verbosity = (Severity.VerbosityLevel < Category.VerbosityLevel ? Severity.VerbosityLevel : Category.VerbosityLevel);
	if (Verbosity >= gGlobalLogVerbosity)
	{
		return;
	}

	String FormattedMessage = String::FormatArgs(Message, Args);

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
	PaddedStringFormatLong.PadRight(35, ' ');

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

	String FinalFormattedMessage = String::FormatArgs("[{Time}] [{Category}] [{Severity}] ({FileInfo}) {Message}\n", LogFormatArgs);
	
	for (auto output : gGlobalLogOutputs)
	{
		output->Emit(FinalFormattedMessage.Data(), Severity.DisplayColorARGB);
	}
}

void LogOutput::GlobalEmitF(const LogSeverity& Severity, const LogCategory& Category, const char* File, int Line, const char* Message, ...)
{
	int Verbosity = (Severity.VerbosityLevel < Category.VerbosityLevel ? Severity.VerbosityLevel : Category.VerbosityLevel);
	if (Verbosity >= gGlobalLogVerbosity)
	{
		return;
	}

	va_list list;
	va_start(list, Message);
	String FormattedMessage = String::FormatVA(Message, list);
	va_end(list);

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
	PaddedStringFormatLong.PadRight(35, ' ');

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

	String FinalFormattedMessage = String::FormatArgs("[{Time}] [{Category}] [{Severity}] ({FileInfo}) {Message}\n", LogFormatArgs);

	for (auto output : gGlobalLogOutputs)
	{
		output->Emit(FinalFormattedMessage.Data(), Severity.DisplayColorARGB);
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

LogOutput::~LogOutput()
{
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