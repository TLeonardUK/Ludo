// Copyright 2016 TwinDrills
#pragma once

#include "Core/Types/String.h"

namespace Ludo {

/// \brief TODO
struct StackFrame
{
public:		
	intptr_t	Address;
	bool		IsResolved;

	String		Function;
	String		File;
	int			Line;
};

/// \brief TODO
class StackTrace
{
private:
	Array<StackFrame> m_Frames;

public:

	/// \brief TODO
	StackTrace();

	/// \brief TODO
	~StackTrace();

	/// \brief TODO
	int FrameCount();

	/// \brief TODO
	StackFrame GetFrame(int Index, bool bResolveSymbols = false);

	/// \brief TODO
	static void Generate();

};

};