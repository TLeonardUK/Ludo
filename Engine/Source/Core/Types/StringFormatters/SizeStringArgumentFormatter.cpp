// Copyright 2016 TwinDrills

#include "Core/Types/StringFormatters/SizeStringArgumentFormatter.h"

namespace Ludo {

SizeStringArgumentFormatter::SizeStringArgumentFormatter()
{
}

void SizeStringArgumentFormatter::Format(const StringArgument& Argument, String& Output)
{
	const char* Postfix[] = {
		"Bytes",
		"KiB",
		"MiB",
		"GiB",
		"TiB",
		"PiB",
		"EiB",
		"ZiB",
		"YiB",
	};

	double Result = (double)Argument.Value.IntValue;
	int PostfixIndex = 0;

	switch (Argument.Value.Type)
	{
		case StringArgumentValueType::Int:				Result = (double)Argument.Value.IntValue;			break;
		case StringArgumentValueType::UnsignedInt:		Result = (double)Argument.Value.UnsignedIntValue;	break;
		case StringArgumentValueType::Double:			Result = (double)Argument.Value.DoubleValue;		break;
		default:
		{
			ConstantAssert();
			break;
		}
	}

	while (Result > 1024.0f && PostfixIndex < ARRAY_LENGTH(Postfix))
	{
		PostfixIndex++;
		Result /= 1024.0f;
	}

	char Buffer[128];
	sprintf(Buffer, "%.2f %s", Result, Postfix[PostfixIndex]);

	Output = Buffer;
}

}; // namespace Ludo