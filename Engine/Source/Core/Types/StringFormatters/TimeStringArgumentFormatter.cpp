// Copyright 2016 TwinDrills

#include "Core/Types/StringFormatters/TimeStringArgumentFormatter.h"

namespace Ludo {

TimeStringArgumentFormatter::TimeStringArgumentFormatter(const char* Format)
	: m_Format(Format)
{
}

void TimeStringArgumentFormatter::Format(const StringArgument& Argument, String& Output)
{
	// TODO
	UNREFERENCED_PARAMETER(Argument);
	UNREFERENCED_PARAMETER(Output);

}

}; // namespace Ludo