// Copyright 2016 TwinDrills
#pragma once

#include "Core/Types/Error.h"

namespace Ludo {
namespace Platform {

// \brief Defined in engine code, called by the platform entry point
//		  to begin game execution!
extern Error LudoEntryPoint();

/// \brief TODO
void PreInit();

/// \brief TODO
void Init();

/// \brief TODO
void Term();

/// \brief TODO
void DebugBreak();

/// \brief TODO
void Exit();

}; // namespace Platform
}; // namespace Ludo