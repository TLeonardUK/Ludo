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
#include "BuildTool/Private/Reflection/ReflectionFileScanner.h"

#include "Framework/Public/GameEngine.h"

#include "clang-c/index.h"

namespace Ludo {

/// \brief TODO
struct PendingBuildCommand
{
	Path SourcePath;
	Path OutputPath;
	Path DependentsOutputPath;
	String FileId;
};

/// \brief TODO
class BuildReflectionCommand : public Command
{
private:
	Array<String> m_CompileArgs;
	Array<String> m_SourceExtensions;
	Array<String> m_Files;

	Array<PendingBuildCommand> m_PendingBuilds;

	int m_CompiledFiles;
	int m_FailedFiles;
	int m_NewFiles;

	GameEngine* m_Engine;

	enum 
	{
		Version = 1 // Increment to force reflection rebuilds.
	};

private:
	bool BuildFolder(const Path& SourceFolder, const Path& OutputFolder, const Path& BaseOutputFolder, const String& FolderId);
	bool BuildFile(const Path& SourceFile, const Path& OutputFile, const Path& DependentsOutputPath, const String& FileId, bool bTreatAsPch);

	void WriteDatabase(const Path& OutputPath, const String& ProjectName, const Array<String>& Files);

	String GenerateConfigurationHash(const String& ProjectName, const Path& OutputPath, const Array<String>& Configuration);
	bool CheckConfigurationMatches(const String& ProjectName, const Path& OutputPath, const Array<String>& Configuration);
	void WriteConfiguration(const String& ProjectName, const Path& OutputPath, const Array<String>& Configuration);

public:
	BuildReflectionCommand(GameEngine* Engine);

	virtual String GetName();
	virtual void PrintHelp();
	virtual bool ValidateArgs(const Array<String>& Args);
	virtual bool Run(const Array<String>& Args);

};

}; // namespace Ludo