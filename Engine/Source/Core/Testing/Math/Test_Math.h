// Copyright 2016 TwinDrills
#pragma once

#include "Core/Testing/Test.h"

namespace Ludo {

/// \brief TODO
class TestMath : public Test
{
public:
	virtual const char* Name() const override
	{
		return "Math Tests";
	}

	virtual void Run() const override;
};

};