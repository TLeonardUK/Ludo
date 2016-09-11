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

#include "Core/Public/Memory/Endianness.h"
#include "Core/Public/Encodings/Utf16TextEncoding.h"

#include "ThirdParty/Utf8Cpp/source/utf8.h"

namespace Ludo {

Utf16TextEncoding::Utf16TextEncoding(bool bBigEndian)
	: m_BigEndian(bBigEndian)
{
}

bool Utf16TextEncoding::IsNewLine(const Array<uint8>& Buffer) const
{
	int LoByte = 0;
	int HiByte = 1;

	// We will have swapped to little endian during decode, no need to do this.
	/*if (m_BigEndian)
	{
		HiByte = 0;
		LoByte = 1;
	}*/

	if (Buffer.Length() == 2)
	{
		if (
			(Buffer[LoByte] == 0x0  && Buffer[HiByte] == 0xA)	||		// Line feed (unix)
			(Buffer[LoByte] == 0x0  && Buffer[HiByte] == 0xB)	||		// Vertical feed
			(Buffer[LoByte] == 0x0  && Buffer[HiByte] == 0xC)	||		// Form Feed
			(Buffer[LoByte] == 0x20 && Buffer[HiByte] == 0x28)	||		// 0x2028 - Line Seperator
			(Buffer[LoByte] == 0x20 && Buffer[HiByte] == 0x29)  ||		// 0x2029 - Paragraph Seperator
			(Buffer[LoByte] == 0x0  && Buffer[HiByte] == 0x85))			// 0x85 - New Line
		{
			return true;
		}
	}
	else if (Buffer.Length() == 4)
	{
		if ((Buffer[LoByte]     == 0x0 && Buffer[HiByte]     == 0xD) &&
			(Buffer[LoByte + 2] == 0x0 && Buffer[HiByte + 2] == 0xA))
		{
			// 0xA - Carriage Return + Line Feed (Windows)
			return true;
		}
	}

	return false;
}

bool Utf16TextEncoding::IsValid(const Array<uint8>& Buffer) const
{
	// TODO: Add some proper validation here please.

	if ((Buffer.Length() % 2) != 0)
	{
		return false;
	}

	return true;
}

Error Utf16TextEncoding::Encode(const Array<uint8>& InputBuffer, Array<uint8>* OutputBuffer) const
{
	OutputBuffer->Resize(InputBuffer.Length() * 4);
	uint16* Output = utf8::utf8to16(InputBuffer.Data(), InputBuffer.Data() + InputBuffer.Length(), reinterpret_cast<uint16*>(OutputBuffer->Data()));

	uint64 Length = reinterpret_cast<uint8*>(Output) - reinterpret_cast<uint8*>(OutputBuffer->Data());
	OutputBuffer->Resize((int)Length);

	// utf library results in big endian, so swap if little.
	if (!m_BigEndian)
	{
		uint16* Start = reinterpret_cast<uint16*>(OutputBuffer->Data());
		for (int i = 0; i < Length / 2; i++)
		{
			Endianness::SwapEndian(*(Start + i));
		}
	}

	return ErrorType::Success;
}

Error Utf16TextEncoding::Decode(const Array<uint8>& InputBuffer, Array<uint8>* OutputBuffer) const
{
	if (!IsValid(InputBuffer))
	{
		return ErrorType::Corrupt;
	}

	try
	{
		Array<uint8> InputBufferEndianCorrect;
		for (int i = 0; i < InputBuffer.Length(); i++)
		{
			InputBufferEndianCorrect.Push(InputBuffer[i]);
		}

		// utf library assumes big endian, so swap if little.
		if (!m_BigEndian)
		{
			uint16* Start = reinterpret_cast<uint16*>(InputBufferEndianCorrect.Data());
			for (int i = 0; i < InputBufferEndianCorrect.Length() / 2; i++)
			{
				Endianness::SwapEndian(*(Start + i));
			}
		}

		uint16* InputStart = reinterpret_cast<uint16*>(InputBufferEndianCorrect.Data());
		uint16* InputEnd = reinterpret_cast<uint16*>(InputBufferEndianCorrect.Data() + InputBufferEndianCorrect.Length());

		OutputBuffer->Resize(InputBuffer.Length() * 8);
		uint8* Output = utf8::utf16to8(InputStart, InputEnd, OutputBuffer->Data());

		uint64 Length = Output - OutputBuffer->Data();
		OutputBuffer->Resize((int)Length);
	}
	catch (utf8::exception)
	{
		return ErrorType::Corrupt;
	}

	return ErrorType::Success;
}

Array<uint8> Utf16TextEncoding::GetBom() const
{
	Array<uint8> Result;

	if (m_BigEndian)
	{
		Result.Push(0xFF);
		Result.Push(0xFE);
	}
	else
	{
		Result.Push(0xFE);
		Result.Push(0xFF);
	}

	return Result;
}

}; // namespace Ludo