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

/** \brief Defines a severity level for log messages emitted by the game.
 *
 * Final verbosity level of a log is determined as:
 *   LogVerbosity = Min(SeverityVerbosity, CategoryVerbosity)
 *
 * A log is only emitted if LogVerbosity < CurrentVerbosityLevel
 * Default verbosity level during development is MAX_LOG_VERBOSITY,
 * verbosity level during release is usually set to 1 (errors only),
 * suppressing all errors can be done by setting the log verbosity
 * to 0.
 */
struct LogSeverity 
{
public:
    static const int MaxNameLength = 128;

	char			Name[MaxNameLength];
	int				VerbosityLevel;
	unsigned int	DisplayColorARGB;
	bool			bSuppressed;

public:
	LogSeverity(const char* InName, const int InVerbosityLevel, unsigned int InDisplayColorARGB);

};

/** \brief Defines a category of logs.
 *
 * Categories are usually used to differentiate which part of the engine each log
 * comes from. For example there could be a Physics category and a Renderer category.
 *
 * This categorisation allows the developer to filter out the logs they are intrested in
 * at runtime and prevent them getting inundated by log spam.
 */
struct LogCategory
{
public:
    static const int MaxNameLength = 128;

	char	Name[MaxNameLength];
	int		VerbosityLevel;
	bool	bSuppressed;

public:
	LogCategory(const char* InName, const int InVerbosityLevel);

};

/** \brief Base class for all log listeners.
 * 
 * All registered log listeners are invoked each time a log is emitted, their purpose
 * is to do something with the log, typically writing it to stdout, a file or another
 * such place.
 */
struct LogListener
{
private:

    /** \brief Overridden by derived classes, this is called for each message that each
      *        log message that is emitted.
      *
      * \param Message   Message that was logged, any formatting tags will have been removed at this point.
      * \param ColorArgb 32bit Argb value defining the color to write the log with.
      */
	virtual void Emit(const char* Message, const unsigned int ColorArgb) = 0;

public:
	virtual ~LogListener();

    // These are internal (and unsupported) functions called by LD_LOG, do not call directly.
	static void GlobalEmit(const LogSeverity& Severity, const LogCategory& Category, const char* File, int Line, const char* Message);
	static void GlobalEmitF(const LogSeverity& Severity, const LogCategory& Category, const char* File, int Line, const char* Message, StringArgumentList& Args);
	static void GlobalEmitF(const LogSeverity& Severity, const LogCategory& Category, const char* File, int Line, const char* Message, ...);

    /** \brief Defines the verbosity of log messages that should be output. 
     *
     * \param Verbosity Level of verbosity of log messages.
     *
     * Please view the LogSeverity classes documentation to see how logs are filtered
     * based on the provided verbosity level.
     */
	static void SetLogVerbosity(int Verbosity);

    /** \brief Gets the current logging verbosity.
     *
     * \returns Current logging verbosity level.
     *
     * Please view the LogSeverity classes documentation to see how logs are filtered
     * based on the provided verbosity level.
     */
	static int  GetLogVerbosity();

    /// \brief Registers this log listener so it recieves future log messages.
	void Register();

    /// \brief Unregisters this log listener so it no longer recieves log messages.
    void Unregister();

};

/// \brief Maximum log level, if log verbosity is set to this, everything will be emitted.
#define LD_MAX_LOG_VERBOSITY 0x7FFFFFFF

/// \brief When added to a cpp file this implements the given log severity.
#define LD_LOG_SEVERITY_IMPLEMENT(Name, VerbosityLevel, DisplayColor) \
	LogSeverity Log##Name(#Name, VerbosityLevel, DisplayColor);

/// \brief When added to a header file this declares the given log severity.
#define LD_LOG_SEVERITY_DECLARE(Name, VerbosityLevel, DisplayColor) \
	extern LogSeverity Log##Name;

/// \brief When added to a cpp file this implements the given log category.
#define LD_LOG_CATEGORY_IMPLEMENT(Name, VerbosityLevel) \
	LogCategory Log##Name(#Name, VerbosityLevel);

/// \brief When added to a header file this declares the given log category.
#define LD_LOG_CATEGORY_DECLARE(Name, VerbosityLevel)  \
	extern LogCategory Log##Name;

/** \brief Emits the given log message.
 *
 * \tparam Category Category to emit message under.
 * \tparam Severity Category to emit message as.
 * \tparam Message  Actual log message. 
 */
#define LD_LOG(Category, Severity, Message) \
	LogListener::GlobalEmit(Log##Severity, Log##Category, __FILE__, __LINE__, Message);

 /** \brief Emits the given log message, formatted with printf style formatting tags.
  *
  * \tparam Category Category to emit message under.
  * \tparam Severity Category to emit message as.
  * \tparam Message  Actual log message.
  * \tparam ...      Varidic arguments that can be inserted into Message using printf tags.
  */
#define LD_LOGF(Category, Severity, Message, ...) \
	LogListener::GlobalEmitF(Log##Severity, Log##Category, __FILE__, __LINE__, Message, ##__VA_ARGS__);

// Declare all the default log severities.
#define LOG_SEVERITY LD_LOG_SEVERITY_DECLARE
#include "Core/Private/Debug/LogSeverities.inc"
#undef LOG_SEVERITY

#define LOG_CATEGORY LD_LOG_CATEGORY_DECLARE
#include "Core/Private/Debug/LogCategories.inc"
#undef LOG_CATEGORY

} // namespace Ludo
