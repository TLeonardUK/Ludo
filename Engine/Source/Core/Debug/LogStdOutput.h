// Copyright 2016 TwinDrills
#pragma once

#include "Core/Debug/Log.h"

namespace Ludo {

struct LogStdOutput : public LogOutput
{
private:
	virtual void Emit(const char* Message) override;
};

};