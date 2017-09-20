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
\brief A collection of records for an individual module.

This class is primarily a way of grouping individual reflection records based
on modules, it allows easily registering and unregistering records in mass.
*/
class RecordModule
{
private:

	static Array<RecordModule*> m_Modules;

public:

	// \brief Destructors.
	virtual ~RecordModule();

	// \brief Registers all records contained in this module.
	virtual void RegisterRecords() = 0;

	// \brief Unregisters all records contained in this module.
	virtual void UnregisterRecords() = 0;

	/*
	\brief		Gets the name of this module.
	\returns	The name of this module as a hashed string id.
	*/
	virtual StringId GetName() const = 0;

public:

	/*
	\brief		Gets a list of all modules that are registered.
	\returns	List of all modules that are registered.
	*/
	static Array<RecordModule*> GetModules();

	/*
	\brief		Tries to find the module with the given name.
	\returns	If module with the name exists pointer will be returned, otherwise nullptr.
	*/
	static RecordModule* FindModule(const StringId& Name);

	/*
	\brief		Registers a module as being available.
	\param		ModulePtr The module to be registered.
	*/
	static void RegisterModule(RecordModule* ModulePtr);

	/*
	\brief		Registers a module that is no longer available.
	\param		ModulePtr The module to be unregistered.
	*/
	static void UnregisterModule(RecordModule* ModulePtr);

};

}; // namespace Ludo
