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

#include "Framework/Public/EngineCore.h"

namespace Ludo {

// \brief Defined in engine code, called by the platform entry point
//		  to begin game execution!
extern int LudoEntryPoint();

/// \brief TODO
enum class EnginePath
{
	ExecutableFile,		// Same as Environemnt::GetSpecialPath.
	TempDirectory,		// Same as Environemnt::GetSpecialPath.
	SaveDataDirectory,	// Same as Environemnt::GetSpecialPath.
	ProjectDirectory,	// Path to the project directory that the game engine is currently running, don't cache as this can change.
	EngineDirectory,	// Path to the engine directory.
	RootDirectory,		// Path to the highest root level of the Ludo engine (the one with the Build/Engine/Projects/Tools folders).
};

/// \brief TODO
class GameEngine
{
private:
	EngineCore* m_EngineCore;

	Path m_ProjectDirectory;
	Path m_EngineDirectory;
	Path m_RootDirectory;

private:

	/// \brief TODO
	void InitPlatform();

	/// \brief TODO
	void TermPlatform();

	/// \brief TODO
	void InitLogging();

	/// \brief TODO
	void TermLogging();

	/// \brief TODO
	void InitReflection();

	/// \brief TODO
	void TermReflection();

	/// \brief TODO
	void EmitBuildInfo();

	/// \brief TODO
	Error LoadEngineCore();

	/// \brief TODO
	void UnloadEngineCore();

	/// \brief TODO
	Error UpdatePaths();

public:

	/// \brief TODO
	int EntryPoint();

	/// \brief TODO
	Path GetEnginePath(EnginePath PathType);
		
};

}; // namespace Ludo