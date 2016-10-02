/*
MicroBuild
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

#include "Commands/BuildMeta.h"
#include "Commands/LinkMeta.h"
#include "Schemas/Plugin/PluginInterface.h"
#include "Core/Helpers/Strings.h"

#include "Core/Public/Environment/Environment.h"

MicroBuildPlugin();

namespace MicroBuild {

bool PluginMain(IPluginInterface* pluginInterface)
{
	pluginInterface->SetName("Ludo Metadata Generator");
	pluginInterface->SetDescription("Responsible for generating reflection metadata for ludo projects.");

	pluginInterface->RegisterCommand(new BuildMetaCommand(pluginInterface));
	pluginInterface->RegisterCommand(new LinkMetaCommand(pluginInterface));

	// When a project file is processed we add in some extra build and link
	// post-build commands to call the BuildMeta and LinkMeta commands when
	// the user builds the projects.
	pluginInterface->RegisterCallback(EPluginEvent::PostProcessProjectFile, [](PluginEventData* Data) {
		PluginPostProcessProjectFileData* projectData = Data->Get<PluginPostProcessProjectFileData>();

		bool isEnabled = projectData->File->GetCastedValue<bool>("Plugin.MetaGeneration", "IsEnabled", false);
		if (isEnabled)
		{
			// Add an include directory for the output directory.
			Platform::Path outputDir = projectData->File->GetCastedValue<Platform::Path>("Plugin.MetaGeneration", "OutputDirectory", "");
			projectData->File->SetOrAddValue("SearchPaths", "IncludeDirectory", outputDir.ToString());

			// Command to build metadata for each individual file in project.
			projectData->File->SetOrAddValue("PreBuildCommands", "Command", "\"$(Target.MicroBuildExecutable)\" BuildMeta \"$(Workspace.File)\" \"$(Project.Name)\" \"$(Target.Configuration)\" \"$(Target.Platform)\" --silent");

			// Command to link metadata from dependent projects for inclusion in output binary.
			projectData->File->SetOrAddValue("PreBuildCommands", "Command", "\"$(Target.MicroBuildExecutable)\" LinkMeta \"$(Workspace.File)\" \"$(Project.Name)\" \"$(Target.Configuration)\" \"$(Target.Platform)\" --silent");
		}

		return true;
	});

	return true;
}

}; // namespace Ludo