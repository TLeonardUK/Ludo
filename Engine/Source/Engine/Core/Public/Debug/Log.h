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
#pragma once

namespace Ludo {

/// Final verbosity level of a log is determined as:
///		LogVerbosity = Min(SeverityVerbosity, CategoryVerbosity)
///
/// A log is only emitted if LogVerbosity < CurrentVerbosityLevel
/// Default verbosity level during development is MAX_LOG_VERBOSITY, 
/// verbosity level during release is usually set to 1 (errors only), 
/// suppressing all errors can be done by setting the log verbosity
/// to 0.
///
/// Individual categories can also be suppressed on an individual basis
/// to control spam if neccessary. All their properties can be 
/// configured at runtime.

struct StringArgumentList;

/// \brief TODO
struct LogSeverity 
{
public:
	enum
	{
		MaxNameLength = 128,
	};

	char			Name[MaxNameLength];
	int				VerbosityLevel;
	unsigned int	DisplayColorARGB;
	bool			bSuppressed;

public:
	LogSeverity(const char* InName, const int InVerbosityLevel, unsigned int InDisplayColorARGB);

};

/// \brief TODO
struct LogCategory
{
public:
	enum
	{
		MaxNameLength = 128,
	};

	char	Name[MaxNameLength];
	int		VerbosityLevel;
	bool	bSuppressed;

public:
	LogCategory(const char* InName, const int InVerbosityLevel);

};

/// \brief TODO
struct LogOutput
{
private:
	virtual void Emit(const char* Message, const unsigned int ColorArgb) = 0;

public:
	virtual ~LogOutput();

	static void GlobalEmit(const LogSeverity& Severity, const LogCategory& Category, const char* File, int Line, const char* Message);
	static void GlobalEmitF(const LogSeverity& Severity, const LogCategory& Category, const char* File, int Line, const char* Message, StringArgumentList& Args);
	static void GlobalEmitF(const LogSeverity& Severity, const LogCategory& Category, const char* File, int Line, const char* Message, ...);

	static void SetLogVerbosity(int Verbosity);
	static int  GetLogVerbosity();

	void Register();
	void Unregister();

};

/// \brief TOD#O
#define MAX_LOG_VERBOSITY 0x7FFFFFFF

/// \brief TODO
#define LD_LOG_SEVERITY_IMPLEMENT(Name, VerbosityLevel, DisplayColor) \
	LogSeverity Log##Name(#Name, VerbosityLevel, DisplayColor);

#define LD_LOG_SEVERITY_DECLARE(Name, VerbosityLevel, DisplayColor) \
	extern LogSeverity Log##Name;

/// \brief TODO
#define LD_LOG_CATEGORY_IMPLEMENT(Name, VerbosityLevel) \
	LogCategory Log##Name(#Name, VerbosityLevel);

#define LD_LOG_CATEGORY_DECLARE(Name, VerbosityLevel)  \
	extern LogCategory Log##Name;

/// \brief TODO
#define LD_LOG(Category, Severity, Message) \
	LogOutput::GlobalEmit(Severity, Category, __FILE__, __LINE__, Message);

#define LD_LOGF(Category, Severity, Message, ...) \
	LogOutput::GlobalEmitF(Severity, Category, __FILE__, __LINE__, Message, ##__VA_ARGS__);

// Declare all the default log severities.
#define LOG_SEVERITY LD_LOG_SEVERITY_DECLARE
#include "Core/Private/Debug/LogSeverities.inc"
#undef LOG_SEVERITY

#define LOG_CATEGORY LD_LOG_CATEGORY_DECLARE
#include "Core/Private/Debug/LogCategories.inc"
#undef LOG_CATEGORY

} // namespace Ludo
