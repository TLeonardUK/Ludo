// Copyright 2016 TwinDrills
#pragma once

#include "Core/Types/Error.h"
#include "Core/Debug/LogStdOutput.h"

namespace Ludo {

// \brief Defined in engine code, called by the platform entry point
//		  to begin game execution!
extern Error LudoEntryPoint();

/// \brief TODO
class GameEngine
{
private:
	LogStdOutput m_StdLoggingOutput;

private:

	/// \brief TODO
	void InitPlatform();

	/// \brief TODO
	void TermPlatform();

	/// \brief TODO
	void InitLogging();

	/// \brief TODO
	void TermLogging();

	/// \brief TODO
	void EmitBuildInfo();

public:

	/// \brief TODO
	Error EntryPoint();
		
};

}; // namespace Ludo