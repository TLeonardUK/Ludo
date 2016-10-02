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

#include "Core/Public/IO/Process.h"

namespace Ludo {

uint64 Process::Position()
{
	NotImplemented();
	return 0;
}

uint64 Process::Length()
{
	NotImplemented();
	return 0;
}

void Process::Seek(uint64 Position)
{
	LD_UNUSED_PARAMETER(Position);
	NotImplemented();
}

bool Process::CanRead()
{
	return true;
}

bool Process::CanWrite()
{
	return true;
}

}; // namespace Ludo