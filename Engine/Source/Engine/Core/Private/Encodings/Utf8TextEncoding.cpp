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

#include "Core/Public/Encodings/Utf8TextEncoding.h"

#include "ThirdParty/Utf8Cpp/source/utf8.h"

namespace Ludo {
    
// ************************************************************************************************

Utf8TextEncoding::Utf8TextEncoding()
{
}

// ************************************************************************************************

bool Utf8TextEncoding::IsNewLine(const Array<uint8>& Buffer) const
{
	if (Buffer.Length() == 1)
	{
		if (Buffer[0] == 0xA || // Line feed (Unix)
			Buffer[0] == 0xB || // Vertical Feed
			Buffer[0] == 0xC)   // Form Feed
		{			
			return true;
		}
	}
	else if (Buffer.Length() == 2)
	{
		if (Buffer[0] == 0xD &&
			Buffer[1] == 0xA)
		{	
			// 0xA - Carriage Return + Line Feed (Windows)
			return true;
		}
		else if (Buffer[0] == 0xC2 &&
			     Buffer[1] == 0x85)
		{
			// 0x85 New Line
			return true;
		}
	}
	else if (Buffer.Length() == 3)
	{
		if (Buffer[0] == 0xE2 &&
			Buffer[1] == 0x80 &&
			Buffer[2] == 0xA8)
		{
			// 0x2028 - Line Seperator
			return true;
		}
		else if (Buffer[0] == 0xE2 &&
				 Buffer[1] == 0x80 &&
				 Buffer[2] == 0xA9)
		{
			// 0x2029 - Paragraph Seperator
			return true;
		}
	}

	return false;
}

// ************************************************************************************************

bool Utf8TextEncoding::IsValid(const Array<uint8>& Buffer) const
{	
	return utf8::is_valid(Buffer.Data(), Buffer.Data() + Buffer.Length());
}

// ************************************************************************************************

Error Utf8TextEncoding::Encode(const Array<uint8>& InputBuffer, Array<uint8>* OutputBuffer) const
{
	// Input is always utf8, so this is a no-op.
	*OutputBuffer = InputBuffer;
	return ErrorType::Success;
}

// ************************************************************************************************

Error Utf8TextEncoding::Decode(const Array<uint8>& InputBuffer, Array<uint8>* OutputBuffer) const
{
	try
	{
		OutputBuffer->Resize(InputBuffer.Length());
		uint8* EndPtr = utf8::replace_invalid(InputBuffer.Data(), InputBuffer.Data() + InputBuffer.Length(), OutputBuffer->Data());

		int Size = (int)(EndPtr - OutputBuffer->Data());
		OutputBuffer->Resize(Size);
	}
	catch (utf8::exception)
	{
		return ErrorType::Corrupt;
	}

	return ErrorType::Success;
}

// ************************************************************************************************

Array<uint8> Utf8TextEncoding::GetBom() const
{
	Array<uint8> Result;
	return Result;
}

// ************************************************************************************************

}; // namespace Ludo