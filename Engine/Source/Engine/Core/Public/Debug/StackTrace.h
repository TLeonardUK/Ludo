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

/// \brief TODO
struct StackFrame
{
public:		
	intptr		Address;
	bool		IsResolved;

	String		Function;
	String		File;
	int			Line;

public:

	/// \brief TODO
	Error Resolve();

};

/// \brief TODO
class StackTrace
{
private:
	Array<StackFrame> m_Frames;

public:

	/// \brief TODO
	typedef void* ExceptionRecord;

	/// \brief TODO
	StackTrace();

	/// \brief TODO
	~StackTrace();

	/// \brief TODO
	int FrameCount();

	/// \brief TODO
	StackFrame GetFrame(int Index);

	/// \brief TODO
	static Error Generate(StackTrace& Trace, ExceptionRecord Record = nullptr);

};

};