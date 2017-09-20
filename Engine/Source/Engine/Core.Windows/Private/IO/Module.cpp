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

#include "PCH.h"

#include "Core.Windows/Private/Platform/Helper/Errors.h"

using namespace Ludo;
using namespace Ludo::Windows;

namespace Ludo {

// ************************************************************************************************

class ModuleImpl
{
private:
	HMODULE m_Module;

public:

	ModuleImpl(HMODULE Handle)
		: m_Module(Handle)
	{
	}

	ModuleImpl()
		: m_Module(nullptr)
	{
	}

	~ModuleImpl()
	{
		if (m_Module != nullptr)
		{
			Close();
		}
	}

	Error Open(const Path& Path) 
	{
		m_Module = LoadLibraryA(Path.ToString().Data());
		if (m_Module == nullptr)
		{
			LogLastSystemError("LoadLibrary");
			return ErrorType::Failure;
		}
		return ErrorType::Success;
	}

	void Close() 
	{
		if (m_Module != nullptr)
		{
			FreeLibrary(m_Module);
			m_Module = nullptr;
		}
	}

	intptr GetExportAddress(const String& Name) 
	{
		return (intptr)GetProcAddress(m_Module, Name.Data());
	}

	void SetHandle(HMODULE Handle)
	{
		m_Module = Handle;
	}

};

// ************************************************************************************************

Module* Module::Self()
{
	static Module SelfModule;
	static bool bSelfModuleInit = false;
	if (!bSelfModuleInit)
	{
		SelfModule.GetImpl<ModuleImpl>()->SetHandle(GetModuleHandle(nullptr));
		bSelfModuleInit = true;
	}

	return &SelfModule;
}

// ************************************************************************************************

Module::Module()
{
	CreateImpl<PlatformMemoryArena, ModuleImpl>();
}

// ************************************************************************************************

Module::~Module()
{
	DestroyImpl<PlatformMemoryArena, ModuleImpl>();
}

// ************************************************************************************************

ErrorType Module::Open(const Path& Path)
{
	return GetImpl<ModuleImpl>()->Open(Path);
}

// ************************************************************************************************

void Module::Close()
{
	GetImpl<ModuleImpl>()->Close();
}

// ************************************************************************************************

intptr Module::GetExportAddress(const String& Name)
{
	return GetImpl<ModuleImpl>()->GetExportAddress(Name);
}

// ************************************************************************************************

}; // namespace Ludo