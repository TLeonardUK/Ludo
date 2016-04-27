// Copyright 2016 TwinDrills

#include "Core/Debug/LogStdOutput.h"

#include <stdio.h>

namespace Ludo {

void LogStdOutput::Emit(const char* Message) 
{
	printf("%s", Message);
}

}; // namespace Ludo