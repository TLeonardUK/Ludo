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

#include "Core/Public/Reflection/Record.h"
#include "Core/Public/Reflection/Attribute.h"
#include "Core/Public/Reflection/Class.h"

namespace Ludo {

Array<Record*> Record::m_Records;
Map<StringId, Record*> Record::m_RecordsByFullName;
Map<StringId, Record*> Record::m_RecordsByName;
Map<StringId, Record*> Record::m_RecordsByMetaTypeInfo;

// ************************************************************************************************

Record::Record()
{
}

// ************************************************************************************************

Record::~Record()
{
	for (Attribute* Attr : m_Attributes)
	{
		LD_ARENA_DELETE(ReflectionMemoryArena, Attr);
	}
	m_Attributes.Empty();
}

// ************************************************************************************************

StringId Record::GetName()
{
	return m_Name;
}

// ************************************************************************************************

StringId Record::GetFullName()
{
	return m_FullyQualifiedName;
}

// ************************************************************************************************

StringId Record::GetMangledName()
{
	return m_MangledName;
}

// ************************************************************************************************

RecordLocation Record::GetLocation()
{
	return m_Location;
}

// ************************************************************************************************

Array<Attribute*> Record::GetAttributes()
{
	return m_Attributes;
}

// ************************************************************************************************

Record* Record::FindByName(const StringId& Name)
{
	Record* Result = nullptr;

	// Don't do this, causes issues with Constructor/Class/etc names that are the same
	// and only differ by scope. Always use full name.
	// if (!m_TypesByName.FindValue(Name, Result))
	// {
	//     m_TypesByFullName.FindValue(Name, Result);
	// }

	m_RecordsByFullName.FindValue(Name, Result);
	return Result;
}

// ************************************************************************************************

void Record::FinalizeRegistration()
{
	for (Record* type : m_Records)
	{
		type->OnPostRegistration();
	}
	for (Record* type : m_Records)
	{
		type->OnBuildTree();
	}	
}

// ************************************************************************************************

void Record::Teardown()
{
}

// ************************************************************************************************

void Record::OnPostRegistration()
{
}

// ************************************************************************************************

void Record::OnBuildTree()
{
}

// ************************************************************************************************

}; // namespace Ludo