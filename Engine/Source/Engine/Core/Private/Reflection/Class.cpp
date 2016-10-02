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

#include "Core/Public/Reflection/Class.h"
#include "Core/Public/Reflection/Type.h"
#include "Core/Public/Reflection/Method.h"

namespace Ludo {

Class::Class()
{
}

void* Class::NewInstance(IAllocator* Allocator)
{
	LD_UNUSED_PARAMETER(Allocator);
	Assert("Attempt to create instance of class with non-overriden NewInstance method.");
	return nullptr;
}

Method* Class::FindMethod(const StringId& Name, Type* ReturnType, Array<Type*> ParameterTypes)
{
	for (Method* Mth : m_Methods)
	{
		if (Mth->GetName() == Name)
		{
			Array<Parameter> MethodParameters = Mth->GetParameters();
			if (MethodParameters.Length() == ParameterTypes.Length())
			{
				bool bMatch = true;

				for (int i = 0; i < MethodParameters.Length(); i++)
				{
					if (MethodParameters[i].DataType != ParameterTypes[i])
					{
						bMatch = false;
						break;
					}
				}

				if (Mth->GetReturnType() != ReturnType)
				{
					bMatch = false;
				}

				if (bMatch)
				{
					return Mth;
				}
			}
		}
	}

	return nullptr;
}

Field* Class::FindField(const StringId& Name, Type* DataType)
{
	for (Field* Fld : m_Fields)
	{
		if (Fld->GetName() == Name && Fld->GetDataType() == DataType)
		{
			return Fld;
		}
	}

	return nullptr;
}

bool Class::IsAbstract()
{
	return m_bAbstract;
}

Array<Class*> Class::GetBaseClasses()
{
	return m_BaseClasses;
}

Array<Class*> Class::GetDerivedClasses()
{
	return m_DerivedClasses;
}

Array<Field*> Class::GetFields()
{
	return m_Fields;
}

Array<Method*> Class::GetMethods()
{
	return m_Methods;
}

}; // namespace Ludo