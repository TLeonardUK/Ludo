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

/*
typedef std::function<bool(EngineInterface& Output)> CreateGameInterfaceFunction_t;

Module Own = Module::Self();
CreateGameInterfaceFunction_t CreateFunction = Own.GetFunction<CreateGameInterfaceFunction_t>("CreateGameInterface");
GameInterface* GameInterace = CreateFunction();
*/

/// \brief TODO
class Module :
	public IPimpl,
	public INotCopyable
{
public:

	/// \brief TODO
	Module();

	/// \brief TODO
	~Module();

	/// \brief TODO
	ErrorType Open(const Path& Path);

	/// \brief TODO
	void Close();

	/// \brief TODO
	intptr GetExportAddress(const String& Name);

	/// \brief TODO
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

	/// \brief TODO
	static Module* Self();

};

};