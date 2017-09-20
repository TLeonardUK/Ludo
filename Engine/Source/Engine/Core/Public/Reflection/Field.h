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

/*
\brief Represents a record for an individual field in a class.

Contains general information on the field and permits the getting
or setting of fields on individual class instances.
*/
class Field 
	: public Record
{
protected:
	Type* m_DataType;

	bool m_bStatic;

protected:

public:

	// \brief Constructors
	Field();

};

}; // namespace Ludo

