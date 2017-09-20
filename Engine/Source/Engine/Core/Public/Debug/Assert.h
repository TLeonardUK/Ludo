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

struct StringArgumentList;

/** \brief Shows a warning message and terminates the application.
 *
 * \param Message Stringified code that failed.
 * \param File    Name of file that assert failed in.
 * \param Line    Line of file that assert failed on.
 *
 * This is the internal function that is called when an assert fails.
 * When invoked this shows the message provided along with the file and line location.
 */
void AssertFailed(const char* Message, const char* File, int Line);

/** \brief Shows a warning message and terminates the application.
 *
 * \param Message Stringified code that failed.
 * \param File    Name of file that assert failed in.
 * \param Line    Line of file that assert failed on.
 * \param Format  User defined message explaining what caused failure.
 *
 * This is the internal function that is called when an assert fails.
 * When invoked this shows the message provided along with the file and line location.
 */
void AssertFailed(const char* Message, const char* File, int Line, const char* Format);

/** \brief Shows a warning message and terminates the application.
 *
 * \param Message Stringified code that failed.
 * \param File    Name of file that assert failed in.
 * \param Line    Line of file that assert failed on.
 * \param Format  User defined message explaining what caused failure.
 * \param Args    Additional arguments that can be inserted into the Format message
 *                using printf formatting tags.
 *
 * This is the internal function that is called when an assert fails.
 * When invoked this shows the message provided along with the file and line location.
 */
void AssertFailedF(const char* Message, const char* File, int Line, const char* Format, StringArgumentList& Args);

/** \brief Shows a warning message and terminates the application. 
 *
 * \param Message Stringified code that failed.
 * \param File    Name of file that assert failed in.
 * \param Line    Line of file that assert failed on.
 * \param Format  User defined message explaining what caused failure.
 * \param ..      Varidic arguments arguments that can be inserted into the Format message
 *                using printf formatting tags.
 *
 * This is the internal function that is called when an assert fails.
 * When invoked this shows the message provided along with the file and line location.
 */
void AssertFailedF(const char* Message, const char* File, int Line, const char* Format, ...);

}; // namespace Ludo

#if !defined(LD_ASSERTS_ENABLED)

/** \brief Checks a condition, if it evaluates to false a warning message 
 *        is shown and the application terminates.
 *
 * \tparam Condition Condition to evaluate.
 */
#define LD_ASSERT(Condition) \
	if (!(Condition)) \
	{ \
		Ludo::AssertFailed(#Condition, __FILE__, __LINE__); \
	}

/** \brief Checks a condition, if it evaluates to false a warning message
 *        is shown and the application terminates.
 *
 * \tparam Condition Condition to evaluate.
 * \tparam Message   Message to show describing the failure.
 */
#define LD_ASSERT_MSG(Condition, Message) \
	if (!(Condition)) \
	{ \
		Ludo::AssertFailed(#Condition, __FILE__, __LINE__, Message); \
	}

/** \brief Checks a condition, if it evaluates to false a warning message
 *        is shown and the application terminates.
 *
 * \tparam Condition Condition to evaluate.
 * \tparam Format    Printf style formatting string for message describing
 *                   the failure.
 * \param ..         Varidic arguments arguments that can be inserted into the Format message
 *                   using printf formatting tags.
 */
#define LD_ASSERT_MSGF(Condition, Format, ...) \
	if (!(Condition)) \
	{ \
		Ludo::AssertFailedF(#Condition, __FILE__, __LINE__, Format, ##__VA_ARGS__); \
	}

/// \brief Shows a warning message and the application terminates.
#define LD_CONSTANT_LD_ASSERT() \
	Ludo::AssertFailed("ConstantAssert", __FILE__, __LINE__);

/** \brief Shows a warning message and the application terminates.
 *
 * \tparam Message   Message to show describing the failure.
 */
#define LD_CONSTANT_ASSERT_MSG(Message) \
	Ludo::AssertFailed("ConstantAssert", __FILE__, __LINE__, Message); 

/** \brief Shows a warning message and the application terminates.
 *
 * \tparam Format    Printf style formatting string for message describing
 *                   the failure.
 * \param ..         Varidic arguments arguments that can be inserted into the Format message
 *                   using printf formatting tags.
 */
#define LD_CONSTANT_ASSERT_MSGF(format, ...) \
	Ludo::AssertFailedF("ConstantAssert", __FILE__, __LINE__, format, ##__VA_ARGS__); 

/** \brief To be placed in unimplemented functions.
 * 
 * Shows a warning message saying the function is not implemented and then
 * terminates the application.
 */
#define LD_NOT_IMPLEMENTED() \
	LD_CONSTANT_ASSERT_MSG("Invoked non-implemented function.");

/** \brief To be placed in pure-virtual functions.
 *
 * Shows a warning message saying the function is pure-virtual and then
 * terminates the application.
 */
#define LD_PURE_VIRTUAL() \
	LD_CONSTANT_ASSERT_MSG("Invoked non-implemented pure-virtual function.");

#else

#define LD_ASSERT(cond) 
#define LD_ASSERT_MSG(cond, format) 
#define LD_ASSERT_MSGF(cond, format, ...) 

#define LD_CONSTANT_LD_ASSERT() 
#define LD_CONSTANT_ASSERT_MSG(format) 
#define LD_CONSTANT_ASSERT_MSGF(format, ...) 

#define LD_NOT_IMPLEMENTED() 
#define LD_PURE_VIRTUAL() 

#endif
