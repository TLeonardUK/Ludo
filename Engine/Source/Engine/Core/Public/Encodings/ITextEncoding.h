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

/// \brief TODO
class ITextEncoding
{
public:

	/// \brief TODO
	virtual bool IsNewLine(const Array<uint8>& Buffer) const = 0;

	/// \brief TODO
	virtual bool IsValid(const Array<uint8>& Buffer) const = 0;

	/// \brief Encodes the given buffer of utf8 text into the given encoding.
	virtual Error Encode(const Array<uint8>& InputBuffer, Array<uint8>* OutputBuffer) const = 0;
	 
	/// \brief Decodes the given buffer of encoded text into utf8.
	virtual Error Decode(const Array<uint8>& InputBuffer, Array<uint8>* OutputBuffer) const = 0;

	/// \brief TODO
	virtual Array<uint8> GetBom() const = 0;

	/// \brief TODO
	Error Encode(const String& Value, Array<uint8>& Buffer);

	/// \brief TODO
	Error Decode(String& Value, const Array<uint8>& Buffer);


};

};