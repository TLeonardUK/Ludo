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
    
/** \brief Wraps a stream and provides additional functionality for reading and writing
 *         to it in a textural format with a given encoding.
 *
 * Always use asyncronous streams when possible.
 */
class TextStream 
    : public StreamWrapper
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
    
	/** \brief Reads a single codepoint in the text streams encoding
     *         out of the stream.
     *
     *  \param  Buffer Byte buffer to store full codepoint in.
     *
     *  \returns Error value indicating success or failure.
     */
	Error ReadCodePoint(Array<uint8>& Buffer);

public:
    
	/** \brief Constructs the text stream.
     *
     *  \param  BaseStream Base stream to redirect internal IO to.
     *  \param  Encoding   Encoding to read and write textural data in.
     */
	TextStream(IStream* BaseStream, ITextEncoding* Encoding = nullptr);

	virtual ~TextStream();
    
	/** \brief Writes a line of formatted text to the stream.
     *
     *  \param  Format   Printf format of line to write.
     *  \param  Encoding Additional arguments to substitute into the Format parameter.
     *
     *  \returns Error value indicating success or failure.
     */
	Error WriteLine(String Format, ...);

	/// \brief Indents any lines written out by one tab level.
	void Indent();

    /// \brief Undents any lines written out by one tab level.
	void Undent();
    
	/** \brief Reads a line of text from the stream.
     *
     *  \param  Result Reference to string to store read line into.
     *
     *  \returns Error value indicating success or failure.
     */
	Error ReadLine(String& Result);

};

}; // namespace Ludo