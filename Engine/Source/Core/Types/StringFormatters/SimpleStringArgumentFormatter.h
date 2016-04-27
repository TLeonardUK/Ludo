// Copyright 2016 TwinDrills
#pragma once

#include "Core/Types/String.h"

namespace Ludo {

/// \brief TODO
struct SimpleStringArgumentFormatter
	: public StringArgumentFormatter
{
private:
	int m_PadLeftLength;
	String m_PadLeftValue;

	int m_PadRightLength;
	String m_PadRightValue;

	int m_NumberPrecision;

public:

	/// \brief TODO
	SimpleStringArgumentFormatter();

	/// \brief TODO
	void PadLeft(const int Length, const String& Padding);

	/// \brief TODO
	void PadRight(const int Length, const String& Padding);

	/// \brief TODO
	void SetNumberPrecision(const int Precision);

	/// \brief TODO
	virtual void Format(const StringArgument& Argument, String& Output) override;

};

};