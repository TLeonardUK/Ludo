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

namespace Ludo {
namespace Endianness {
    
/** \brief Swaps endian of an arbitrary type. This inverts the order of bytes, changing MSB to LSB and vis-versa.
 * 
 *  \tparam BaseType Base type of object to swap endian of. Typically compiler infered.
 *
 *  \param Data Reference to object to swap endian of.
 */
template <typename BaseType>
void SwapEndian(BaseType& Data)
{
	uint8* Ptr = reinterpret_cast<uint8*>(&Data);
	int Len = sizeof(BaseType);
	for (int i = 0; i < Len / 2; i++)
	{
		uint8 Tmp = Ptr[i];
		Ptr[i] = Ptr[(Len - i) - 1];
		Ptr[(Len - i) - 1] = Tmp;
	}
}

}; // namespace Endianness
}; // namespace Ludo
