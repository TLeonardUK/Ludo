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

/// \brief TODO
class TypeModule
{
private:
	static Array<TypeModule*> m_Modules;

public:
	virtual ~TypeModule();

	/// \brief TODO
	virtual void RegisterTypes() = 0;

	/// \brief TODO
	virtual void UnregisterTypes() = 0;

	/// \brief TODO
	virtual StringId GetName() const = 0;

public:

	/// \brief TODO
	static Array<TypeModule*> GetModules();

	/// \brief TODO
	static TypeModule* FindModule(const StringId& Name);

	/// \brief TODO
	static void RegisterModule(TypeModule* Ptr);

	/// \brief TODO
	static void UnregisterModule(TypeModule* Ptr);

};

}; // namespace Ludo
