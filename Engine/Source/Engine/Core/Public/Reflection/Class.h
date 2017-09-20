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

#include "Core/Public/Reflection/Record.h"
#include "Core/Public/Reflection/Method.h"
#include "Core/Public/Reflection/Field.h"

namespace Ludo {

class Method;

/*
\brief Primitive types supported for class template parameters.
*/
enum class EClassTemplateParameterType
{
	Type,
	Integer
};

/*
\brief Parameter of a class template type.

A template type can either be a typename or an integer, we do not support
other template types supported by C++.
*/
struct ClassTemplateParameter
{
public:

	// \brief Type of template argument, can be a type or an integer.
	EClassTemplateParameterType ParameterType;

	// \brief Value if base-type is an integral argument.
	int64_t Value;

	// \brief The actual resolved type of the template argument
	Type* BaseType;

	// \brief Constructor.
	ClassTemplateParameter(EClassTemplateParameterType InType, int64_t InValue, Type* InBaseType);

};

/* 
\brief Represents a record for an individual class.

Contains general information on the class - methods, fields, properties, 
and allows the instantiation of new instances.
*/
class Class 
	: public Record
{
protected:

	Array<Method*> m_Methods;
	Array<Field*> m_Fields;
	Array<Class*> m_BaseClasses;

	bool m_bAbstract;
	bool m_bStruct;
	bool m_bTemplate;
	bool m_bTemplateInstance;

	Array<ClassTemplateParameter> m_TemplateParameters;

protected:

public:

	// \brief Constructors
	Class();

};

}; // namespace Ludo
