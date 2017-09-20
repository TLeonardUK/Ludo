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

#include "Core/Public/Types/Array.h"
#include "Core/Public/Types/Error.h"
#include "Core/Public/Types/String.h"

namespace Ludo {

/** \brief Base class for text-encoding implementations.
 *
 * Text encoding classes provide functionality to encode or decode byte arrays or strings
 * to and from a given encoding.
 */
class ITextEncoding
{
public:

	/** \brief  Checks if the bytes in the buffer represent a new line character sequence.
     *
     *  \param  Buffer  Buffer of bytes to check.
     *
     *  \return Returns true if its a new line.
     */
	virtual bool IsNewLine(const Array<uint8>& Buffer) const = 0;

    /** \brief Checks if the bytes in a buffer represent are valid for the encoding.
     *  \param  Buffer  Buffer of bytes to check.
     *
     *  \return Returns true if byte buffer is valid for the encoding.
     */
	virtual bool IsValid(const Array<uint8>& Buffer) const = 0;

    /** \brief  Encodes the given buffer of utf8 text into the given encoding.
     *
     *  \param  InputBuffer    Buffer of utf8 bytes to encode.
     *  \param  OutputBuffer   Output buffer that resulting encoding should be written to.
     *
     *  \return Error value determining success.
     */
	virtual Error Encode(const Array<uint8>& InputBuffer, Array<uint8>* OutputBuffer) const = 0;

    /** \brief Decodes the given buffer of encoded text into utf8.
     *
     *  \param  InputBuffer    Buffer of bytes in the given encoding to decode.
     *  \param  OutputBuffer   Output buffer that resulting utf8 text should be written to.
     *
     *  \return Error value determining success.
     */
	virtual Error Decode(const Array<uint8>& InputBuffer, Array<uint8>* OutputBuffer) const = 0;

    /** \brief If encoding uses a BOM, this will return the BOM used for this encoding.
    *
    *   \return Byte order mark for this encoding.
    */
	virtual Array<uint8> GetBom() const = 0;

    /** \brief Encodes a string to a sequence of bytes in the encoding.
    *
    *  \param  Value  String to encode.
    *  \param  Buffer Output buffer that resulting encoding should be written to.
    *
    *  \return Error value determining success.
    */
	Error Encode(const String& Value, Array<uint8>& Buffer);

    /** \brief Decodes a sequence of bytes in the encoding, to a string.
    *
    *  \param  Value  String value result should be written to.
    *  \param  Buffer Buffer of bytes in encoding to decode.
    *
    *  \return Error value determining success.
    */
	Error Decode(String& Value, const Array<uint8>& Buffer);


};

};