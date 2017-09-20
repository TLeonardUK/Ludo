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
#include "Core/Public/Meta/IPimpl.h"
#include "Core/Public/Meta/Traits/INotCopyable.h"
#include "Core/Public/Types/Error.h"
#include "Core/Public/IO/Path.h"

namespace Ludo {

/** \brief Represents a module of executable code on the 
 *         platforms local file system.
 *
 * Example of using this to load a function from an external module:
 *
 *   typedef std::function<bool(EngineInterface& Output)> CreateGameInterfaceFunction_t;
 *   Module Own = Module::Self();
 *   CreateGameInterfaceFunction_t CreateFunction = Own.GetFunction<CreateGameInterfaceFunction_t>("CreateGameInterface");
 *   GameInterface* GameInterace = CreateFunction();
 */
class Module :
	public IPimpl,
	public INotCopyable
{
public:

	Module();
	~Module();

    /** \brief Opens the module on the platforms file system.
     *
     *  \param  Path Path to the module on the local file system.
     *
     *  \returns Error value indicating success or failure.
     */
	Error Open(const Path& Path);

    /// \brief Closes access to the module. This is implicitly called during destruction.
	void Close();
    
    /** \brief Gets a pointer to the exported function from the module.
     *
     *  \param Name Name of exported address to get a pointer to..
     *
     *  \returns Pointer to exported function, or null if not found.
     */
	intptr GetExportAddress(const String& Name);
    
    /** \brief Gets a function in the form of a function signature typedef.
     *
     *  \param Name       Name of exported function to retrieve.
     *  \tparam Signature Signature of function to retrieve.
     *
     *  \returns Pointer to function in the format of the typedef provided.
     */
	template <typename Signature>
	Signature GetFunction(const String& Name)
	{
		intptr Address = GetExportAddress(Name);
		if (Address != 0)
		{
			return (Signature)Address;
		}
		return nullptr;
	}
    
    /** \brief Retrieves a module representing the currently executing module.
     *
     *  \returns Currently executing module.
     */
	static Module* Self();

};

};