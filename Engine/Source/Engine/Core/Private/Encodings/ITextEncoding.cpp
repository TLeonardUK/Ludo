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

#include "Core/Public/Encodings/ITextEncoding.h"

namespace Ludo {

// ************************************************************************************************

Error ITextEncoding::Encode(const String& Value, Array<uint8>& Buffer)
{
	Array<uint8> Bytes;
	Bytes.Reserve(Value.ByteLength());

	memcpy(Bytes.Data(), Value.Data(), Value.ByteLength());

	return Encode(Bytes, &Buffer);
}

// ************************************************************************************************

Error ITextEncoding::Decode(String& Value, const Array<uint8>& Buffer)
{
	Array<uint8> Bytes;

	Error Err = Decode(Buffer, &Bytes);
	if (Err.Failed())
	{
		return Err;
	}

	Value = String(reinterpret_cast<char*>(Bytes.Data()), Bytes.Length());

	return ErrorType::Success;
}

// ************************************************************************************************

}; // namespace Ludo