// Copyright 2016 TwinDrills
#pragma once

#include "Core/Types/String.h"

namespace Ludo {

/// \brief TODO
struct SizeStringArgumentFormatter
	: public StringArgumentFormatter
{
private:

public:

	/// \brief TODO
	SizeStringArgumentFormatter();

	/// \brief TODO
	virtual void Format(const StringArgument& Argument, String& Output) override;

};

};