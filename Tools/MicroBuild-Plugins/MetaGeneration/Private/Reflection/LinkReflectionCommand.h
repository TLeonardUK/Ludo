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

#include "BuildTool/Private/Command.h"

#include "Framework/Public/GameEngine.h"

#include "clang-c/index.h"

namespace Ludo {

/// \brief TODO
class LinkReflectionCommand : public Command
{
private:
	Array<String> m_CompileArgs;
	Array<String> m_SourceExtensions;
	Array<String> m_Files;

	GameEngine* m_Engine;

	enum
	{
		Version = 1 // Increment to force reflection rebuilds.
	};

private:
	String GenerateConfigurationHash(const String& ProjectName, const Path& OutputPath, const Array<String>& Configuration);
	bool CheckConfigurationMatches(const String& ProjectName, const Path& OutputPath, const Array<String>& Configuration);
	void WriteConfiguration(const String& ProjectName, const Path& OutputPath, const Array<String>& Configuration);

	Error WriteDatabase(const String& ProjectName, const Path& OutputPath, const Array<String>& LinkedProjects);

public:
	LinkReflectionCommand(GameEngine* Engine);

	virtual String GetName();
	virtual void PrintHelp();
	virtual bool ValidateArgs(const Array<String>& Args);
	virtual bool Run(const Array<String>& Args);

};

}; // namespace Ludo