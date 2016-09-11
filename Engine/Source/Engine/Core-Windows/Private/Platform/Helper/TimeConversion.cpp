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

#include "Core-Windows/Private/Platform/Helper/TimeConversion.h"

#define WINDOWS_TICK		10000000LL
#define SEC_TO_UNIX_EPOCH	11644473600LL

namespace Ludo {
namespace Windows {

uint64 FileTimeToUnixTimestamp(const FILETIME& FileTime)
{
	ULARGE_INTEGER LargeInt;
	LargeInt.LowPart = FileTime.dwLowDateTime;
	LargeInt.HighPart = FileTime.dwHighDateTime;

	return (LargeInt.QuadPart / WINDOWS_TICK) - SEC_TO_UNIX_EPOCH;
}

}; // namespace Windows
}; // namespace Ludo