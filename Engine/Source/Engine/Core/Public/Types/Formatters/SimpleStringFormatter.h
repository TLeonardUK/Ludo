/*
Ludo Game Engine
Copyright (C) 2016 TwinDrills

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "Core/Public/Types/String.h"

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