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

#include "Core/Public/Reflection/RecordModule.h"

namespace Ludo {

Array<RecordModule*> RecordModule::m_Modules;

// ************************************************************************************************

RecordModule::~RecordModule()
{
}

// ************************************************************************************************

Array<RecordModule*> RecordModule::GetModules()
{
	return m_Modules;
}

// ************************************************************************************************

RecordModule* RecordModule::FindModule(const StringId& Name)
{
	int Index = m_Modules.FindMatch([Name] (RecordModule* const* Ptr) -> bool 
	{ 
		return (*Ptr)->GetName() == Name;
	});

	if (Index != m_Modules.NoIndex)
	{
		return m_Modules[Index];
	}

	return nullptr;
}

// ************************************************************************************************

void RecordModule::RegisterModule(RecordModule* Ptr)
{
	LD_ASSERT(!m_Modules.Contains(Ptr));
	m_Modules.Add(Ptr);
}

// ************************************************************************************************

void RecordModule::UnregisterModule(RecordModule* Ptr)
{
	m_Modules.Remove(Ptr);
}

// ************************************************************************************************

}; // namespace Ludo