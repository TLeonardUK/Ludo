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

#include "Commands/BuildDocs.h"
#include "Schemas/Plugin/PluginInterface.h"
#include "Core/Helpers/Strings.h"

#include "Core/Public/Environment/Environment.h"

MicroBuildPlugin();

namespace MicroBuild {

BuildDocsCommand* g_buildDocsCommand;

bool OnPluginUnload(IPluginInterface* pluginInterface)
{
	pluginInterface->UnregisterCommand(g_buildDocsCommand);

	delete g_buildDocsCommand;

	return true;
}

bool OnPluginLoad(IPluginInterface* pluginInterface)
{
	pluginInterface->SetName("Ludo Documentation Generator");
	pluginInterface->SetDescription("Responsible for generating documentation files for ludo projects.");

	g_buildDocsCommand = new BuildDocsCommand(pluginInterface);

	pluginInterface->RegisterCommand(g_buildDocsCommand);

	// When a project file is processed we add in some extra build and link
	// post-build commands to call the BuildDocs the user builds the projects.
	pluginInterface->RegisterCallback(EPluginEvent::PostProcessProjectFile, [](PluginEventData* Data) {
		PluginPostProcessProjectFileData* projectData = Data->Get<PluginPostProcessProjectFileData>();

		bool isEnabled = projectData->File->GetCastedValue<bool>("Plugin.DocGeneration", "IsEnabled", false);
		if (isEnabled)
		{
			projectData->File->SetOrAddValue("PreBuildCommands", "Command", "\"$(Target.MicroBuildExecutable)\" BuildDocs \"$(Workspace.File)\" \"$(Project.Name)\" \"$(Target.Configuration)\" \"$(Target.Platform)\" --silent");
		}

		return true;
	});

	return true;
}

}; // namespace Ludo