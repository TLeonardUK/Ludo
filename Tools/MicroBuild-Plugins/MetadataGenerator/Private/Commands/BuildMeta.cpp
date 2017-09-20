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
 
#include <memory>
#include <chrono>

#include "Commands/BuildMeta.h"

#include "Actions/ExtractMetadataFromHeader.h"
#include "Actions/WriteHeaderMetadataToFile.h"
#include "Actions/WriteSourceMetadataToFile.h"
#include "Actions/WriteProjectMetadataToFile.h"

#include "Schemas/Plugin/PluginInterface.h"

#include "Core/Commands/CommandLineParser.h"
#include "Core/Commands/CommandPathArgument.h"
#include "Core/Commands/CommandStringArgument.h"
#include "Core/Commands/CommandFlagArgument.h"

namespace MicroBuild {

BuildMetaCommand::BuildMetaCommand(IPluginInterface* pluginInterface)
	: m_pluginInterface(pluginInterface)
{
	SetName("buildmeta");
	SetShortName("bm");
	SetDescription("Builds metadata for the given project file.");

	CommandPathArgument* workspaceArg = new CommandPathArgument();
	workspaceArg->SetName("Workspace File Path");
	workspaceArg->SetShortName("w");
	workspaceArg->SetDescription("Path to workspace file to build metadata for.");
	workspaceArg->SetRequired(true);
	workspaceArg->SetPositional(true);
	workspaceArg->SetDefault("");
	workspaceArg->SetExpectsExisting(true);
	workspaceArg->SetExpectsDirectory(false);
	workspaceArg->SetOutput(&m_workspaceFilePath);
	RegisterArgument(workspaceArg);

	CommandStringArgument* projectArg = new CommandStringArgument();
	projectArg->SetName("Project Name");
	projectArg->SetShortName("n");
	projectArg->SetDescription("Name of project in the workspace to build metadata for.");
	projectArg->SetRequired(true);
	projectArg->SetPositional(true);
	projectArg->SetDefault("");
	projectArg->SetOutput(&m_projectName);
	RegisterArgument(projectArg);

	CommandStringArgument* configuration = new CommandStringArgument();
	configuration->SetName("Configuration");
	configuration->SetShortName("c");
	configuration->SetDescription("Defines the configuration of the project that should be built.");
	configuration->SetRequired(true);
	configuration->SetPositional(true);
	configuration->SetDefault("");
	configuration->SetOutput(&m_configuration);
	RegisterArgument(configuration);

	CommandStringArgument* platform = new CommandStringArgument();
	platform->SetName("Platform");
	platform->SetShortName("p");
	platform->SetDescription("Defines the platform this project should be built for.");
	platform->SetRequired(true);
	platform->SetPositional(true);
	platform->SetDefault("");
	platform->SetOutput(&m_platform);
	RegisterArgument(platform);

	CommandFlagArgument* rebuild = new CommandFlagArgument();
	rebuild->SetName("Rebuild");
	rebuild->SetShortName("r");
	rebuild->SetDescription("Operation will not be performed incrementally even if it appears nothing has changed.");
	rebuild->SetRequired(false);
	rebuild->SetPositional(false);
	rebuild->SetDefault(false);
	rebuild->SetOutput(&m_rebuild);
	RegisterArgument(rebuild);
}

bool BuildMetaCommand::Invoke(CommandLineParser* parser)
{
	MB_UNUSED_PARAMETER(parser);

	Log(LogSeverity::Verbose, "Generating metadata...\n");

	std::shared_ptr<IWorkspace> workspace = m_pluginInterface->ReadWorkspace(m_workspaceFilePath);
	if (!workspace)
	{
		Log(LogSeverity::Fatal, "Failed to open workspace configuration: %s\n", m_workspaceFilePath.ToString().c_str());
		return false;
	}

	std::shared_ptr<IProject> project = workspace->GetProject(m_projectName);
	if (!project)
	{
		Log(LogSeverity::Fatal, "No project in workspace found with name: %s\n", m_projectName.c_str());
		return false;
	}

	std::shared_ptr<IProjectState> state = project->GetState(m_configuration, m_platform);
	if (!state)
	{
		Log(LogSeverity::Fatal, "Project does not have matching build state: %s|%s\n", m_configuration.c_str(), m_platform.c_str());
		return false;
	}

	// Grab metadata configuration data.
	ProjectFile& config = state->GetConfigFile();

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
		outputDir
	);

	// Make sure generated files exists so includes work when parsing.
	int outOfDateFiles = 0;

	for (auto& info : fileInfos)
	{
		if (m_rebuild)
		{
			info.bOutOfDate = true;
		}

		if (info.bOutOfDate)
		{
			outOfDateFiles++;
		}

		if (!info.GeneratedHeaderPath.Exists())
		{
			if (!info.GeneratedHeaderPath.CreateAsFile())
			{
				Log(LogSeverity::Fatal, "Failed to create generated file: %s\n", info.GeneratedHeaderPath.ToString().c_str());
				return false;
			}
		}
		if (!info.GeneratedSourcePath.Exists())
		{
			if (!info.GeneratedSourcePath.CreateAsFile())
			{
				Log(LogSeverity::Fatal, "Failed to create generated file: %s\n", info.GeneratedSourcePath.ToString().c_str());
				return false;
			}
		}
	}

	// Generate all the actions required to build metadata.
#if 0//def MB_DEBUG_BUILD
	JobScheduler scheduler(1);
#else
	JobScheduler scheduler(Platform::GetConcurrencyFactor());
#endif
	bool bBuildFailed = false;

	auto startTime = std::chrono::high_resolution_clock::now();

	JobHandle groupJob = CreateBuildJob(scheduler, fileInfos, &bBuildFailed, config);
	scheduler.Enqueue(groupJob);
	scheduler.Wait(groupJob);
	
	auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
	auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();
	if (!bBuildFailed)
	{
		if (outOfDateFiles == 0)
		{
			Log(LogSeverity::Info, "Metadata up to date\n");
		}
		else
		{
			Log(LogSeverity::Info, "Metadata generated, scanned %i files in %.1f seconds\n", outOfDateFiles, elapsedMs / 1000.0f);
		}
	}

	return !bBuildFailed;
}

JobHandle BuildMetaCommand::CreateBuildJob(
	JobScheduler& scheduler,
	std::vector<MetadataFileInfo>& fileInfos,
	bool* bBuildFailed,
	ProjectFile& project)
{
	// Generate build jobs for each individual metadata file 
	// that is out of date.
	JobHandle buildGroupJob = scheduler.CreateJob();

	for (MetadataFileInfo& info : fileInfos)
	{
		if (info.bOutOfDate)
		{
			JobHandle buildFileJob = scheduler.CreateJob([&info, bBuildFailed, &project]() { 

				// Extract data from header.
				ExtractMetadataFromHeader scanAction(info);
				*bBuildFailed = *bBuildFailed || !scanAction.Execute();

				// Write out data to .generated.h file.
				WriteHeaderMetadataToFile writeAction(info);
				*bBuildFailed = *bBuildFailed || !writeAction.Execute();

				// Write out data to .generated.cpp file.
				WriteSourceMetadataToFile writeSourceAction(info, project);
				*bBuildFailed = *bBuildFailed || !writeSourceAction.Execute();

				// Store out the updated file manifest.
				*bBuildFailed = *bBuildFailed || !info.StoreManifest();
			});
			scheduler.AddDependency(buildGroupJob, buildFileJob);
		}
	}

	// When all jobs are complete, we build the database file.
	JobHandle databaseBuildJob = scheduler.CreateJob([&fileInfos, bBuildFailed]() {

		// Write out the project metatdata database to its respective .h and .cpp.
		WriteProjectMetadataToFile writeAction(fileInfos);
		*bBuildFailed = *bBuildFailed || !writeAction.Execute();

	});
	scheduler.AddDependency(databaseBuildJob, buildGroupJob);

	return buildGroupJob;
}

}; // namespace MicroBuild