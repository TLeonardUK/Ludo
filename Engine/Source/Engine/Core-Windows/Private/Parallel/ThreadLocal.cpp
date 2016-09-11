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

#include "Core-Windows/Private/Platform/Helper/Errors.h"

using namespace Ludo;
using namespace Ludo::Windows;

namespace Ludo {

int NewTlsSlot()
{
	int Slot = (int)TlsAlloc();
	Assert(Slot != TLS_OUT_OF_INDEXES);
	return Slot;
}

void FreeTlsSlot(int Slot)
{
	TlsFree((DWORD)Slot);
}

void SetTlsSlot(int Slot, void* Data)
{
	TlsSetValue((DWORD)Slot, Data);
}

void* GetTlsSlot(int Slot)
{
	return TlsGetValue((DWORD)Slot);
}

}; // namespace Ludo
