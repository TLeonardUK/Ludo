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

#include "Core/Public/IO/Streams/TextStream.h"
#include "Core/Public/Environment/Environment.h"

#include "Core/Public/Math/Standard/Standard.h"

#include <stdarg.h>

namespace Ludo {

TextStream::TextStream(IStream* BaseStream, ITextEncoding* Encoding)
	: StreamWrapper(BaseStream)
	, m_IndentLevel(0)
	, m_Encoding(Encoding)
	, m_Utf16TextEncoding(false)
	, m_Utf32TextEncoding(false)
{
	if (Encoding == nullptr)
	{
		// Try and read BOM from file to determine encoding.
		if (BytesLeft() >= 2)
		{
			uint8 Bom[4];
			uint64 BomLength = Math::Min(BytesLeft(), 4llu);

			uint64 ValidBombLength = 0;

			Error Err = Read((void*)Bom, BomLength);
			if (Err.Succeeded())
			{
				if (BomLength >= 2)
				{
					// Little Endian utf16
					if (Bom[0] == 0xFE && Bom[1] == 0xFF)
					{
						m_Utf16TextEncoding = Utf16TextEncoding(false);
						m_Encoding = &m_Utf16TextEncoding;
						ValidBombLength = 2;
					}
					// Big Endian utf16
					else if (Bom[0] == 0xFF && Bom[1] == 0xFE)
					{
						m_Utf16TextEncoding = Utf16TextEncoding(true);
						m_Encoding = &m_Utf16TextEncoding;
						ValidBombLength = 2;
					}
					else if (BomLength >= 3)
					{
						// Utf8
						if (Bom[0] == 0xEF && Bom[1] == 0xBB && Bom[2] == 0xBF)
						{
							m_Encoding = &m_Utf8TextEncoding;
							ValidBombLength = 3;
						}
						else if (BomLength >= 4)
						{
							// Little Endian utf32
							if (Bom[0] == 0x00 && Bom[1] == 0x00 && Bom[2] == 0xFE && Bom[3] == 0xFF)
							{
								m_Utf32TextEncoding = Utf32TextEncoding(false);
								m_Encoding = &m_Utf32TextEncoding;
								ValidBombLength = 4;
							}
							// Big Endian utf32
							else if (Bom[0] == 0xFF && Bom[1] == 0xFE && Bom[2] == 0x00 && Bom[3] == 0x00)
							{
								m_Utf32TextEncoding = Utf32TextEncoding(true);
								m_Encoding = &m_Utf32TextEncoding;
								ValidBombLength = 4;
							}
						}
					}			
				}
			}

			Seek(Position() - (BomLength - ValidBombLength));
		}

		// Fallback to utf8 if we can't figure it out.
		if (m_Encoding == nullptr)
		{
			m_Encoding = &m_Utf8TextEncoding;
		}
	}

	// Write byte order mark if required.
	if (CanWrite()) 
	{
		Array<uint8> bom = m_Encoding->GetBom();
		if (!bom.IsEmpty())
		{
			Write(bom.Data(), bom.Length());
		}
	}
}

TextStream::~TextStream()
{
	// Don't delete, auto-detected are RAII, passed encodings are responsibility of caller.
	m_Encoding = nullptr;
}

Error TextStream::WriteLine(String Format, ...)
{
	va_list list;
	va_start(list, Format);

	String Formatted = String::FormatVA(String('\t', m_IndentLevel) + Format + Environment::NewLineSequence, list);	

	Array<uint8> RawUtf8((uint8*)Formatted.Data(), Formatted.ByteLength());
	Array<uint8> Encoded;

	Error Result = m_Encoding->Encode(RawUtf8, &Encoded);
	if (Result.Succeeded())
	{
		Result = Write((void*)Encoded.Data(), Encoded.Length());
	}

	va_end(list);

	return Result;
}

void TextStream::Indent()
{
	m_IndentLevel++;
}

void TextStream::Undent()
{
	Assert(m_IndentLevel > 0);

	m_IndentLevel--;
}

Error TextStream::ReadCodePoint(Array<uint8>& Buffer)
{
	char NextChar;

	m_IntermediateCodepoint.Empty();

	// Read based on encoding.
	while (true)
	{
		Error Err = Read((void*)&NextChar, 1);
		if (Err.Failed())
		{
			return Err;
		}
		else
		{
			m_IntermediateCodepoint.Push(NextChar);
			if (m_Encoding->IsValid(m_IntermediateCodepoint))
			{
				Err = m_Encoding->Decode(m_IntermediateCodepoint, &Buffer);
				if (Err.Succeeded())
				{
					return ErrorType::Success;
				}
			}
		}
	}

	return ErrorType::Success;
}

Error TextStream::ReadLine(String& Result)
{
	Array<uint8> Codepoint;
	Array<uint8> LastCodepoint;

	m_IntermediateLine.Empty();

	bool bIsEndOfLine = false;

	while (!bIsEndOfLine)
	{
		// Read full codepoint.
		Codepoint.Empty();
		Error SubResult = ReadCodePoint(Codepoint);
		if (SubResult.Failed())
		{
			if (SubResult.GetType() == ErrorType::AtEnd)
			{
				break;
			}
			return SubResult;
		}

		// Is it a breaking codepoint?
		if (Codepoint.Length() > 0)
		{
			// Deal with two-char newlines (\r\n etc).
			if (LastCodepoint.Length() > 0)
			{
				LastCodepoint.Insert(Codepoint, LastCodepoint.Length());
				if (m_Utf8TextEncoding.IsNewLine(LastCodepoint))
				{
					m_IntermediateLine.Pop(LastCodepoint.Length() - Codepoint.Length());
					bIsEndOfLine = true;
				}
			}
			
			// Signal codepoint newline.
			if (!bIsEndOfLine)
			{
				if (m_Utf8TextEncoding.IsNewLine(Codepoint))
				{
					bIsEndOfLine = true;
				}
			}

			if (!bIsEndOfLine)
			{
				m_IntermediateLine.Append(Codepoint);
			}

			LastCodepoint = Codepoint;
		}
	}

	Result = String((char*)m_IntermediateLine.Data(), m_IntermediateLine.Length());
	return ErrorType::Success;
}

}; // namespace Ludo