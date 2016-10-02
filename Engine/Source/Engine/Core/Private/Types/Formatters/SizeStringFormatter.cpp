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

#include "Core/Public/Types/Formatters/SizeStringFormatter.h"

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

	while (Result > 1024.0f && PostfixIndex < LD_ARRAY_LENGTH(Postfix))
	{
		PostfixIndex++;
		Result /= 1024.0f;
	}

	char Buffer[128];
	sprintf(Buffer, "%.2f %s", Result, Postfix[PostfixIndex]);

	Output = Buffer;
}

}; // namespace Ludo