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

#include "Core/Public/Types/String.h"

namespace Ludo {

/// \brief Represents an individual frame within a callstack trace.
struct StackFrame
{
public:		
	intptr		Address;
	bool		IsResolved;

	String		Function;
	String		File;
	int			Line;

public:

	/** \brief Symbolicates the stack frame information.
     *
     * Symbolication involves invoking the platform-specific symbol service to 
     * resolve the stack frames function address, this will produce the function name
     * and file location.
     *
     * Calling this can take in the order of hundreds of milliseconds.
     *
     * \returns Error value determining if function was successful.
     */
	Error Resolve();

};

/** \brief Represents a platform-indepdendent stack-trace.
 *
 * Capturing a stack-trace is reasonably quick, however the stack frames
 * will not be symbolicated. If you want symbol information you should call
 * the Resolve method on each StackFrame, which will do a platform-specific
 * lookup to try and resolve the symbol information, which will likely be slow.
 */
class StackTrace
{
private:
	Array<StackFrame> m_Frames;

public:

	/** \brief Platform independent respresentation of a stack frame generated
     *          from an exception. 
     *
     * Instances of this can be passed into the Generate function to produce 
     * a stack trace. 
     */
	typedef void* ExceptionRecord;

	StackTrace();
    ~StackTrace();

	/** \brief   Gets the number of frames in the stack.
     *  \returns The number of frames in the stack.
     */
	int FrameCount();

    /** \brief   Gets a specific frame in the stack.
     *
     *  \param Index Index of the frame to get, frames are ordered from 
     *               most deep, to least.
     *
     *  \returns A reference to the frame at the given index in the stack.
     */
	StackFrame& GetFrame(int Index);

	/** \brief Generates a stack trace either from an exception record
     *         or if none is provided, from the current thread.  
     *
     *  \param Trace  Reference to variable to store trace in.
     *  \param Record Exception record, if generating from an exception
     *                produced stack-trace.
     *
     *  \returns Error value determining if this generation failed or succeeded.
     */
	static Error Generate(StackTrace& Trace, ExceptionRecord Record = nullptr);

};

};