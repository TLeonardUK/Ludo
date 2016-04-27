// Copyright 2016 TwinDrills
#pragma once

#include "Barrier.h"

namespace Ludo {

/// \brief TODO
class Barrier
{
private:
	class Impl;
	Impl* m_Impl;

public:

	/// \brief TODO
	void Reset(int ParticipantCount);

	/// \brief TODO
	void Wait();

};

};