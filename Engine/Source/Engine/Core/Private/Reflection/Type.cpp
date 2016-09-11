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

#include "Core/Public/Reflection/Type.h"
#include "Core/Public/Reflection/Attribute.h"
#include "Core/Public/Reflection/Class.h"

namespace Ludo {

Array<Type*> Type::m_Types;
Map<StringId, Type*> Type::m_TypesByFullName;
Map<StringId, Type*> Type::m_TypesByName;
Map<StringId, Type*> Type::m_TypesByTypeInfo;
Map<StringId, Type*> Type::m_TypesByMetaTypeInfo;

Type::Type()
{
}

Type::~Type()
{
	for (Attribute* Attr : m_Attributes)
	{
		LD_ARENA_DELETE(ReflectionMemoryArena, Attr);
	}
	m_Attributes.Empty();
}

StringId Type::GetName()
{
	return m_Name;
}

StringId Type::GetFullName()
{
	return m_FullName;
}

StringId Type::GetUUID()
{
	return m_UUID;
}

Array<Attribute*> Type::GetAttributes()
{
	return m_Attributes;
}

TypeAccessQualifier Type::GetAccessQualifier()
{
	return m_AccessQualifier;
}

Type* Type::FindByName(const StringId& Name)
{
	Type* Result = nullptr;

	// Don't do this, causes issues with Constructor/Class/etc names that are the same
	// and only differ by scope. Always use full name.
	//if (!m_TypesByName.FindValue(Name, Result))
	//{
	//	m_TypesByFullName.FindValue(Name, Result);
	//}

	m_TypesByFullName.FindValue(Name, Result);
	return Result;
}

Type* Type::FindByTypeInfo(const std::type_info& id)
{
	Type* Result = nullptr;
	StringId Name = StringId::Create(id.name());
	if (!m_TypesByTypeInfo.FindValue(Name, Result))
	{
		m_TypesByMetaTypeInfo.FindValue(Name, Result);
	}
	return Result;
}

/// \brief TODO
void Type::FinalizeRegistration()
{
	for (Type* type : m_Types)
	{
		type->OnPostRegistration();
	}
	for (Type* type : m_Types)
	{
		type->OnBuildClassTree();
	}	
}

/// \brief TODO
void Type::Teardown()
{
}

/// \brief TODO
void Type::OnPostRegistration()
{
}

/// \brief TODO
void Type::OnBuildClassTree()
{
}

/// \brief TODO
Array<Class*> Type::GetDerivedClasses(Class* Base)
{
	Array<Class*> Result;
	for (Type* type : m_Types)
	{
		Class* cls = dynamic_cast<Class*>(type);
		if (cls != nullptr)
		{
			Array<Class*> BaseClasses = cls->GetBaseClasses();
			if (BaseClasses.Contains(Base))
			{
				Result.Add(cls);
			}
		}
	}
	return Result;
}

}; // namespace Ludo