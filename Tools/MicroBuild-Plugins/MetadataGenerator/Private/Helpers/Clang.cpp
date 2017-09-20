/*
MicroBuild
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

#include "Helpers/Clang.h"

namespace MicroBuild {
namespace Clang {

void GetCursorLocation(CXCursor cursor, Platform::Path* filename, int* line)
{
	if (filename != nullptr)
	{
		*filename = "";
	}
	if (line != nullptr)
	{
		*line = 0;
	}

	CXFile locationFile;
	unsigned int unsignedLine;
	CXSourceLocation location = clang_getCursorLocation(cursor);
	clang_getSpellingLocation(location, &locationFile, &unsignedLine, nullptr, nullptr);

	if (locationFile == nullptr)
	{
		return;
	}

	if (filename != nullptr)
	{
		CXString locationFileString = clang_getFileName(locationFile);
		*filename = clang_getCString(locationFileString);
		clang_disposeString(locationFileString);
	}

	if (line != nullptr)
	{
		*line = static_cast<int>(unsignedLine);
	}
}

void LogError(CXCursor source, LogSeverity level, const char* format, va_list list)
{
	std::string formattedMessage = Strings::FormatVa(format, list);

	Platform::Path filename;
	int line;
	GetCursorLocation(source, &filename, &line);

	const char* levelName = "info";
	if (level == LogSeverity::Fatal)
	{
		levelName = "error";
	}
	else if (level == LogSeverity::Warning)
	{
		levelName = "warning";
	}

	std::string finalResult = Strings::Format("%s(%u): %s: %s", filename.ToString().c_str(), line, levelName, formattedMessage.c_str());

	// Make a normal long for the error.
	Log(level, "%s\n", finalResult.c_str());
}

void LogError(CXCursor source, LogSeverity level, const char* format, ...)
{
	va_list list;
	va_start(list, format);
	LogError(source, level, format, list);
	va_end(list);
}

}; // namespace Clang
}; // namespace MicroBuild