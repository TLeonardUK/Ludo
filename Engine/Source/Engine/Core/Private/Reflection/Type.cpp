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
#include "Core/Public/Reflection/Class.h"
#include "Core/Public/Reflection/Enum.h"

namespace Ludo {
    
// ************************************************************************************************

Type::Type(TypePrimitive primitive)
	: m_Primitive(primitive)
	, m_bConst(false)
{
}

// ************************************************************************************************

Type::Type(TypePrimitive primitive, bool bConst)
	: m_Primitive(primitive)
	, m_bConst(bConst)
{
}

// ************************************************************************************************

Type::Type(TypePrimitive primitive, Type* pointerType)
	: m_Primitive(primitive)
	, m_PointerType(pointerType)
	, m_bConst(false)
{
}

// ************************************************************************************************

Type::Type(TypePrimitive primitive, Type* pointerType, bool bConst)
	: m_Primitive(primitive)
	, m_PointerType(pointerType)
	, m_bConst(bConst)
{
}

// ************************************************************************************************

Type::Type(Class* classType)
	: m_Primitive(TypePrimitive::Class)
	, m_Class(classType)
	, m_bConst(false)
{
}

// ************************************************************************************************

Type::Type(Class* classType, bool bConst)
	: m_Primitive(TypePrimitive::Class)
	, m_Class(classType)
	, m_bConst(bConst)
{
}

// ************************************************************************************************

Type::Type(Enum* enumType)
	: m_Primitive(TypePrimitive::Enum)
	, m_Enum(enumType)
	, m_bConst(false)
{
}

// ************************************************************************************************

Type::Type(Enum* enumType, bool bConst)
	: m_Primitive(TypePrimitive::Enum)
	, m_Enum(enumType)
	, m_bConst(bConst)
{
}

// ************************************************************************************************

TypePrimitive Type::GetPrimitive()
{
	return m_Primitive;
}

// ************************************************************************************************

Type* Type::GetPointerType()
{
	return m_PointerType;
}

// ************************************************************************************************

Class* Type::GetClass()
{
	return m_Class;
}

// ************************************************************************************************

Enum* Type::GetEnum()
{
	return m_Enum;
}

// ************************************************************************************************

bool Type::IsConst()
{
	return m_bConst;
}

// ************************************************************************************************

String Type::ToString()
{		
	String result;

	switch (m_Primitive)
	{
	case TypePrimitive::Pointer:
		{
			result = m_PointerType->ToString() + "* ";
			break;
		}
	case TypePrimitive::LValueReference:
		{
			result = m_PointerType->ToString() + "& ";
			break;
		}
	case TypePrimitive::RValueReference:
		{
			result = m_PointerType->ToString() + "&& ";
			break;
		}
	case TypePrimitive::Class:
		{
			result = m_Class->GetFullName().ToString();
			break;
		}
	case TypePrimitive::Enum:
		{
			result = m_Enum->GetFullName().ToString();
			break;
		}
	case TypePrimitive::Void:
		{
			result = "void";
			break;
		}
	case TypePrimitive::Bool:
		{
			result = "bool";
			break;
		}
	case TypePrimitive::Int8:
		{
			result = "signed char";
			break;
		}
	case TypePrimitive::Int16:
		{
			result = "signed short";
			break;
		}
	case TypePrimitive::Int32:
		{
			result = "signed int";
			break;
		}
	case TypePrimitive::Int64:
		{
			result = "signed long long";
			break;
		}
	case TypePrimitive::UInt8:
		{
			result = "unsigned char";
			break;
		}
	case TypePrimitive::UInt16:
		{
			result = "unsigned short";
			break;
		}
	case TypePrimitive::UInt32:
		{
			result = "unsigned int";
			break;
		}
	case TypePrimitive::UInt64:
		{
			result = "unsigned long long";
			break;
		}
	case TypePrimitive::Float:
		{
			result = "float";
			break;
		}
	case TypePrimitive::Double:
		{
			result = "double";
			break;
		}
	default:
		{
			LD_CONSTANT_LD_ASSERT();
		}
	}

	if (m_bConst)
	{
		result += " const";
	}

	return result;
}

// ************************************************************************************************

}; // namespace Ludo