// Copyright 2016 TwinDrills
#pragma once

#include "Core/Testing/Test.h"

namespace Ludo {

/// \brief TODO
class TestArray : public Test
{
public:
	virtual const char* Name() const override
	{
		return "Array Tests";
	}

	virtual void Run() const override;
};

};