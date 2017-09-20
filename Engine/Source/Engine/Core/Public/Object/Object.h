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

#include "Core/Public/Reflection/ReflectionMacros.h"
#include "Core/Public/Object/Object.generated.h"

namespace Ludo {
	        
class Type;

/** \brief Base class for all game objects.
 *
 * The object class provides the basic functionality required to support 
 * our internal reflection and type systems. 
 */
META()
class Object
{
	GENERATED_BODY()

public:

	/** \brief Gets the reflection type for this object.
	 *
	 *	\returns Constant reference to the type instance that represents this class.
	 */
	virtual const Type* GetType();

};

};