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

#include "Core/Public/Types/String.h"

namespace Ludo {

class Enum;
class Class;

// Primitive representation of a Type within qualifiers attached.
enum class TypePrimitive
{
	Pointer,
	LValueReference,
	RValueReference,
	Class,
	Enum,

	Void,
	Bool,
	Int8,
	Int16,
	Int32,
	Int64,
	UInt8,
	UInt16,
	UInt32,
	UInt64,

	Float,
	Double
};

/*
\brief Represents a qualified data type.

A qualified data type is a type as defined in C++, it can be a mixture of 
types - primitives, enum, values, with C++ attributes applied to 
them - const, pointer, references, etc.
*/
class Type
{
private:
	TypePrimitive m_Primitive;
	Type* m_PointerType;
	Class* m_Class;
	Enum* m_Enum;
	bool m_bConst;

public:
	// \brief Constructors.
	Type(TypePrimitive primitive);
	Type(TypePrimitive primitive, bool bConst);
	Type(TypePrimitive primitive, Type* pointerType);
	Type(TypePrimitive primitive, Type* pointerType, bool bConst);
	Type(Class* classType);
	Type(Class* classType, bool bConst);
	Type(Enum* enumType);
	Type(Enum* enumType, bool bConst);

	/*
	\brief		Gets the primitive type of this qualified type.
	\returns	Primitive type of this qualified type.
	*/
	TypePrimitive GetPrimitive();

	/*
	\brief		Gets the type this type points to.
	\returns	Type this type points to.

	This method is only valid if the primitive type is Pointer, LValueReference or 
	RValueReference. If the the primitive type is not, the result is indeterminate.
	*/
	Type* GetPointerType();

	/*
	\brief		Gets the class this type represents.
	\returns	Class this type represents.

	This method is only valid if the primitive type is Class. If
	the the primitive type is not, the result is indeterminate.
	*/
	Class* GetClass();
	
	/*
	\brief		Gets the enum this type represents.
	\returns	Enum this type represents.

	This method is only valid if the primitive type is Enum. If
	the the primitive type is not, the result is indeterminate.
	*/
	Enum* GetEnum();	

	/*
	\brief		Returns true if this type is a constant value.
	\returns	True if this type is a constant value.
	*/
	bool IsConst();

	/*
	\brief		Returns a string representation of this type.
	\returns	A string representation of this type, do not expect this to match how
				the type was defined in the source file as unsupported qualifiers
				may be dropped (volatile, restrict, register, etc).
	*/
	String ToString();

};

}; // namespace Ludo
