// Copyright 2016 TwinDrills
#pragma once

#include "Core/Types/String.h"

#include <Windows.h>

namespace Ludo {
namespace Windows {

String FormatSystemError(::DWORD Error);
void   LogLastSystemError(String SourceFunction);
void   AssertLastSystemError(String SourceFunction);

}; // namespace Windows
}; // namespace Ludo