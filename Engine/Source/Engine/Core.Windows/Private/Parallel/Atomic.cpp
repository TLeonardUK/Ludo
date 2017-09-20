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

namespace Ludo {
namespace Atomic {

// ************************************************************************************************

int Increment(int* Value)
{
	return (int)InterlockedIncrement(reinterpret_cast<LONG*>(Value));
}

// ************************************************************************************************

int Decrement(int* Value)
{
	return (int)InterlockedDecrement(reinterpret_cast<LONG*>(Value));
}

// ************************************************************************************************

int CompareExchange(int* Destination, int Exchange, int Comparand)
{
	return (int)InterlockedCompareExchange(
		reinterpret_cast<LONG*>(Destination),
		static_cast<LONG>(Exchange),
		static_cast<LONG>(Comparand)
	);
}

// ************************************************************************************************

void* CompareExchange(void** Destination, void* Exchange, void* Comparand)
{
	return reinterpret_cast<PVOID>(InterlockedCompareExchangePointer(
		reinterpret_cast<PVOID*>(Destination),
		reinterpret_cast<PVOID>(Exchange),
		reinterpret_cast<PVOID>(Comparand)
	));
}

// ************************************************************************************************

void RelaxCpu()
{
	// Compiles down to _mm_pause normally, so this should be what we want.
	YieldProcessor();
}

// ************************************************************************************************

}; // namespace Atomic
}; // namespace Ludo