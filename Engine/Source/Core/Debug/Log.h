// Copyright 2016 TwinDrills
#pragma once

// This file is included globally, be careful what you put in here.

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
	virtual void Emit(const char* Message) = 0;

public:
	static void GlobalEmit(const LogSeverity& Severity, const LogCategory& Category, const char* File, int Line, const char* Message);
	static void GlobalEmit(const LogSeverity& Severity, const LogCategory& Category, const char* File, int Line, const char* Message, StringArgumentList& Args);

	static void SetLogVerbosity(int Verbosity);
	static int  GetLogVerbosity();

	void Register();
	void Unregister();

};

/// \brief TODO
void LogPrintf(const char* Format, ...);

/// \brief TOD#O
#define MAX_LOG_VERBOSITY 0x7FFFFFFF

/// \brief TODO
#define LogSeverityImplement(Name, VerbosityLevel, DisplayColor) \
	LogSeverity Log##Name(#Name, VerbosityLevel, DisplayColor.ToArgb());

#define LogSeverityDeclare(Name, VerbosityLevel, DisplayColor) \
	extern LogSeverity Log##Name;

/// \brief TODO
#define LogCategoryImplement(Name, VerbosityLevel) \
	LogCategory Log##Name(#Name, VerbosityLevel);

#define LogCategoryDeclare(Name, VerbosityLevel)  \
	extern LogCategory Log##Name;

/// \brief TODO
#define Log(Category, Severity, Message, ...) \
	LogOutput::GlobalEmit(Severity, Category, __FILE__, __LINE__, Message, ##__VA_ARGS__);

// Declare all the default log severities.
#define LOG_SEVERITY LogSeverityDeclare
#include "Core/Debug/LogSeverities.inc"
#undef LOG_SEVERITY

#define LOG_CATEGORY LogCategoryDeclare
#include "Core/Debug/LogCategories.inc"
#undef LOG_CATEGORY

} // namespace Ludo