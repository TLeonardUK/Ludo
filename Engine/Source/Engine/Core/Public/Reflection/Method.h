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

namespace Ludo {

class Type;
class Class;
	
/*
\brief Represents an individual parameter of a class method.

Contains the type, name and other misc information about the parameter.
*/
class Parameter
	: public Record
{
private:
	Type*    m_DataType;

public:

	// \brief Constructors
	Parameter();

	/*
	\brief		Gets the type of this parameter.
	\returns	The type of this parameter.
	*/
	Type* GetType() const;

};

/*
\brief Represents a record for an individual field in a method.

Contains general information on the method and permits the invokation
of methods in individual class instances.
*/
class Method 
	: public Record
{
protected:

	Array<Parameter> m_Parameters;
	Type* m_ReturnType;

	Class* m_Class;

	bool m_bConstructor;
	bool m_bStatic;
	bool m_bConst;
	bool m_bVirtual;
	bool m_bPureVirtual;

public:

	// \brief Constructors
	Method();

};

}; // namespace Ludo

