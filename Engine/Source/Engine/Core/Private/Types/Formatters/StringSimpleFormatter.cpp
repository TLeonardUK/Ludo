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

#include "PCH.h"

#include "Core/Public/Types/Formatters/SimpleStringFormatter.h"

namespace Ludo {

SimpleStringArgumentFormatter::SimpleStringArgumentFormatter()
	: m_PadLeftLength(0)
	, m_PadRightLength(0)
	, m_NumberPrecision(0)
{
}

void SimpleStringArgumentFormatter::PadLeft(const int Length, const String& Padding)
{
	m_PadLeftLength = Length;
	m_PadLeftValue = Padding;
}

void SimpleStringArgumentFormatter::PadRight(const int Length, const String& Padding)
{
	m_PadRightLength = Length;
	m_PadRightValue = Padding;
}

void SimpleStringArgumentFormatter::SetNumberPrecision(const int Precision)
{
	m_NumberPrecision = Precision;
}

void SimpleStringArgumentFormatter::Format(const StringArgument& Argument, String& Output)
{
	char Raw[128];

	switch (Argument.Value.Type)
	{
		case StringArgumentValueType::Int:
		{
			sprintf(Raw, "%lli", Argument.Value.IntValue);
			Output = Raw;
			break;
		}
		case StringArgumentValueType::UnsignedInt:
		{
			sprintf(Raw, "%llu", Argument.Value.IntValue);
			Output = Raw;
			break;
		}
		case StringArgumentValueType::Double:
		case StringArgumentValueType::Float:
		{
			if (m_NumberPrecision != 0)
			{
				char PrecisionFormat[32];
				sprintf(PrecisionFormat, "%%.%if", m_NumberPrecision);
				sprintf(Raw, PrecisionFormat, (Argument.Value.Type == StringArgumentValueType::Float) ? Argument.Value.FloatValue : Argument.Value.DoubleValue);
			}
			else
			{
				sprintf(Raw, "%f", (Argument.Value.Type == StringArgumentValueType::Float) ? Argument.Value.FloatValue : Argument.Value.DoubleValue);
			}
			Output = Raw;
			break;
		}
		case StringArgumentValueType::String:
		{
			Output = Argument.Value.StringValue;
			break;
		}
	}

	if (m_PadLeftLength != 0 && Output.CharLength() < m_PadLeftLength)
	{
		Output = Output.PadLeft(m_PadLeftLength, m_PadLeftValue);
	}

	if (m_PadRightLength != 0 && Output.CharLength() < m_PadRightLength)
	{
		Output = Output.PadRight(m_PadRightLength, m_PadRightValue);
	}
}

}; // namespace Ludo