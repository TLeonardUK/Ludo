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
#include "Core/Public/Encodings/Utf32TextEncoding.h"

#include "ThirdParty/Utf8Cpp/source/utf8.h"

namespace Ludo {
    
// ************************************************************************************************

Utf32TextEncoding::Utf32TextEncoding(bool bBigEndian)
	: m_BigEndian(bBigEndian)
{
}

// ************************************************************************************************

bool Utf32TextEncoding::IsNewLine(const Array<uint8>& Buffer) const
{
	if (Buffer.Length() == 4)
	{
		int Code1 = *reinterpret_cast<uint32*>(Buffer.Data());

		// We will have swapped to little endian during decode, no need to do this.
		/*
		if (m_BigEndian)
		{
			Endianness::SwapEndian(Code1);
		}
		*/

		if (
			Code1 == 0xA	||	// Line feed (unix)
			Code1 == 0xB	||	// Vertical feed
			Code1 == 0xC	||	// Form Feed
			Code1 == 0x2028	||	// 0x2028 - Line Seperator
			Code1 == 0x2029 ||	// 0x2029 - Paragraph Seperator
			Code1 == 0x85)		// 0x85 - New Line
		{
			return true;
		}
	}
	else if (Buffer.Length() == 8)
	{
		int Code1 = *reinterpret_cast<uint32*>(Buffer.Data());
		int Code2 = *reinterpret_cast<uint32*>(Buffer.Data() + 4);

		// We will have swapped to little endian during decode, no need to do this.
		/*
		if (m_BigEndian)
		{
			Endianness::SwapEndian(Code1);
			Endianness::SwapEndian(Code2);
		}
		*/

		if (Code1 == 0xD && Code2 == 0xA)
		{
			// 0xA - Carriage Return + Line Feed (Windows)
			return true;
		}
	}

	return false;
}

// ************************************************************************************************

bool Utf32TextEncoding::IsValid(const Array<uint8>& Buffer) const
{
	// TODO: Add some proper validation here please.

	if ((Buffer.Length() % 4) != 0)
	{
		return false;
	}

	return true;
}

// ************************************************************************************************

Error Utf32TextEncoding::Encode(const Array<uint8>& InputBuffer, Array<uint8>* OutputBuffer) const
{
	OutputBuffer->Resize(InputBuffer.Length() * 8);
	uint32* Output = utf8::utf8to32(InputBuffer.Data(), InputBuffer.Data() + InputBuffer.Length(), reinterpret_cast<uint32*>(OutputBuffer->Data()));

	uint64 Length = reinterpret_cast<uint8*>(Output) - reinterpret_cast<uint8*>(OutputBuffer->Data());
	OutputBuffer->Resize((int)Length);

	// utf library results in big endian, so swap if little.
	if (!m_BigEndian)
	{
		uint32* Start = reinterpret_cast<uint32*>(OutputBuffer->Data());
		for (int i = 0; i < Length / 4; i++)
		{
			Endianness::SwapEndian(*(Start + i));
		}
	}

	return ErrorType::Success;
}

// ************************************************************************************************

Error Utf32TextEncoding::Decode(const Array<uint8>& InputBuffer, Array<uint8>* OutputBuffer) const
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
			uint32* Start = reinterpret_cast<uint32*>(InputBufferEndianCorrect.Data());
			for (int i = 0; i < InputBufferEndianCorrect.Length() / 4; i++)
			{
				Endianness::SwapEndian(*(Start + i));
			}
		}

		uint32* InputStart = reinterpret_cast<uint32*>(InputBufferEndianCorrect.Data());
		uint32* InputEnd = reinterpret_cast<uint32*>(InputBufferEndianCorrect.Data() + InputBufferEndianCorrect.Length());

		OutputBuffer->Resize(InputBufferEndianCorrect.Length() * 8);
		uint8* Output = utf8::utf32to8(InputStart, InputEnd, OutputBuffer->Data());

		uint64 Length = Output - OutputBuffer->Data();
		OutputBuffer->Resize((int)Length);
	}
	catch (utf8::exception)
	{
		return ErrorType::Corrupt;
	}

	return ErrorType::Success;
}

// ************************************************************************************************

Array<uint8> Utf32TextEncoding::GetBom() const
{
	Array<uint8> Result;

	if (m_BigEndian)
	{
		Result.Push(0xFF);
		Result.Push(0xFE);
		Result.Push(0x00);
		Result.Push(0x00);
	}
	else
	{
		Result.Push(0x00);
		Result.Push(0x00);
		Result.Push(0xFE);
		Result.Push(0xFF);
	}

	return Result;
}

// ************************************************************************************************

}; // namespace Ludo