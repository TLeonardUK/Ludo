// Copyright 2016 TwinDrills

#include "Core/Types/StringFormatters/SimpleStringArgumentFormatter.h"

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

	if (m_PadLeftLength != 0 && Output.Length() < m_PadLeftLength)
	{
		Output = Output.PadLeft(m_PadLeftLength, m_PadLeftValue);
	}

	if (m_PadRightLength != 0 && Output.Length() < m_PadRightLength)
	{
		Output = Output.PadRight(m_PadRightLength, m_PadRightValue);
	}
}

}; // namespace Ludo