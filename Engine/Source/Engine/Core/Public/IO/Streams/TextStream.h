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

#include "Core/Public/IO/Streams/StreamWrapper.h"

#include "Core/Public/Encodings/Utf8TextEncoding.h"
#include "Core/Public/Encodings/Utf16TextEncoding.h"
#include "Core/Public/Encodings/Utf32TextEncoding.h"

namespace Ludo {

/// \brief TODO
class TextStream : public StreamWrapper
{
private:
	int m_IndentLevel;

	ITextEncoding* m_Encoding;

	// Automatically detected encodings, here to reduce heap allocations.
	Utf8TextEncoding  m_Utf8TextEncoding;
	Utf16TextEncoding m_Utf16TextEncoding;
	Utf32TextEncoding m_Utf32TextEncoding;

	Array<uint8> m_IntermediateCodepoint;
	Array<uint8> m_IntermediateLine;


protected:
	Error ReadCodePoint(Array<uint8>& Buffer);

public:

	/// \brief TODO
	TextStream(IStream* BaseStream, ITextEncoding* Encoding = nullptr);

	/// \brief TODO
	virtual ~TextStream();

	/// \brief TODO
	Error WriteLine(String Format, ...);

	/// \brief TODO
	void Indent();

	/// \brief TODO
	void Undent();

	/// \brief TODO
	Error ReadLine(String& Result);

};

}; // namespace Ludo