// Copyright 2016 TwinDrills
#pragma once

#include "Core/Types/String.h"

namespace Ludo {

/// \brief TODO
struct TimeStringArgumentFormatter
	: public StringArgumentFormatter
{
private:
	const char* m_Format;

public:

	/// \brief TODO
	TimeStringArgumentFormatter(const char* Format);

	/// \brief TODO
	virtual void Format(const StringArgument& Argument, String& Output) override;

};

};