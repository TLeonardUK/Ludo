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
#include "Core/Log.h"

#include "Core/Public/Environment/Environment.h"

MicroBuildPlugin();

namespace MicroBuild {

BuildMetaCommand* g_buildMetaCommand;
LinkMetaCommand* g_linkMetaCommand;

bool OnPluginUnload(IPluginInterface* pluginInterface)
{
	pluginInterface->UnregisterCommand(g_buildMetaCommand);
	pluginInterface->UnregisterCommand(g_linkMetaCommand);

	delete g_buildMetaCommand;
	delete g_linkMetaCommand;

	return true;
}

bool OnPluginLoad(IPluginInterface* pluginInterface)
{
	pluginInterface->SetName("Ludo Metadata Generator");
	pluginInterface->SetDescription("Responsible for generating reflection metadata for ludo projects.");

	g_buildMetaCommand = new BuildMetaCommand(pluginInterface);
	g_linkMetaCommand = new LinkMetaCommand(pluginInterface);

	pluginInterface->RegisterCommand(g_buildMetaCommand);
	pluginInterface->RegisterCommand(g_linkMetaCommand);

	// When a project file is processed we add in some extra build and link
	// post-build commands to call the BuildMeta and LinkMeta commands when
	// the user builds the projects.
	pluginInterface->RegisterCallback(EPluginEvent::PostProcessProjectFile, [](PluginEventData* Data) {
		PluginPostProcessProjectFileData* projectData = Data->Get<PluginPostProcessProjectFileData>();

		bool isEnabled = projectData->File->GetCastedValue<bool>("Plugin.MetaGeneration", "IsEnabled", false);
		if (isEnabled)
		{
			// Add an include directory for the output directory.
			Platform::Path outputDir = projectData->File->GetUnresolvedCastedValue<Platform::Path>("Plugin.MetaGeneration", "OutputDirectory", "");
			projectData->File->SetOrAddValue("SearchPaths", "IncludeDirectory", outputDir.ToString());

			std::string verboseFlag = "--silent";
			if (LogGetVerbose())
			{
				verboseFlag = "--verbose";
			}

			// Command to build metadata for each individual file in project.
			projectData->File->SetOrAddValue("PreBuildCommands", "Command", Strings::Format("%s %s", "\"$(Target.MicroBuildExecutable)\" BuildMeta \"$(Workspace.File)\" \"$(Project.Name)\" \"$(Target.Configuration)\" \"$(Target.Platform)\"", verboseFlag.c_str()));

			// Command to link metadata from dependent projects for inclusion in output binary.
			projectData->File->SetOrAddValue("PreBuildCommands", "Command", Strings::Format("%s %s", "\"$(Target.MicroBuildExecutable)\" LinkMeta \"$(Workspace.File)\" \"$(Project.Name)\" \"$(Target.Configuration)\" \"$(Target.Platform)\"", verboseFlag.c_str()));
		}

		return true;
	});
	
	// When the internal build tool is running, gather up IbtPopulateCompileFiles the metadata files as well.
	pluginInterface->RegisterCallback(EPluginEvent::IbtPopulateCompileFiles, [](PluginEventData* Data) {
		PluginIbtPopulateCompileFilesData* projectData = Data->Get<PluginIbtPopulateCompileFilesData>();

		bool isEnabled = projectData->File->GetCastedValue<bool>("Plugin.MetaGeneration", "IsEnabled", false);
		if (isEnabled)
		{
			ProjectFile& config = *projectData->File;

			Platform::Path outputDir = config.GetCastedValue<Platform::Path>("Plugin.MetaGeneration", "OutputDirectory", "");
			Platform::Path rootDir = config.GetCastedValue<Platform::Path>("Plugin.MetaGeneration", "RootDirectory", "");
			if (!rootDir.IsDirectory())
			{
				Log(LogSeverity::Fatal, "Root directory for meta generation does not exist: %s\n", rootDir.ToString().c_str());
				return false;
			}
	
			std::vector<MetadataFileInfo> fileInfos = MetadataFileInfo::GetMultipleFileInfos(
				config.Get_Files_File(), 
				config.Get_SearchPaths_IncludeDirectory(),
				config.Get_Defines_Define(),
				rootDir, 
				outputDir,
				true
			);

			for (auto& info : fileInfos)
			{			
				projectData->SourceFiles->push_back(info.GeneratedSourcePath);				
			}
		}

		return true;
	});
	
	return true;
}

}; // namespace Ludo