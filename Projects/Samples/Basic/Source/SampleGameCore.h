// Copyright 2016 TwinDrills
#pragma once

#include "Properties.h"
#include "Framework/GameCore.h"

namespace Game {

/// \brief This game core controls this samples high level iteraction with the engine. Responsible
///		   for setting up and shutting down all game side code.
class SampleGameCore : public Ludo::GameCore
{
protected:
	virtual bool Init() override;
	virtual bool Term() override;
};

}; // namespace Game