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

#include "Core/Public/Types/Array.h"
#include "Core/Public/Types/Map.h"
#include "Core/Public/Types/String.h"
#include "Core/Public/Types/StringId.h"

#include <typeindex>

namespace Ludo {

/* 
\brief Base class for all attributes that can be applied to metadata records.

Attributes are attached to individual reflection records using the META macro. Each attribute
in the META macro expands to an instantiation of an attribute called NameAttribute, with all parameters
given passed to its constructor.

e.g.
	
	META(DisplayName("My Display Name"), Serializable)
	class ...

Weill attach the following attributes to the the classes record:
	
	new DisplayNameAttribute("My Display Name")
	new SerializableAttribute()
*/
class Attribute
{
public:

	// \brief Constructors
	virtual ~Attribute();

};

}; // namespace Ludo
