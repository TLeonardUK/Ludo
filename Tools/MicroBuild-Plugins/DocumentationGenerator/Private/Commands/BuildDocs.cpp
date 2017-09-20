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

#include "Commands/BuildDocs.h"

#include "Schemas/Plugin/PluginInterface.h"

#include "Core/Commands/CommandLineParser.h"
#include "Core/Commands/CommandPathArgument.h"
#include "Core/Commands/CommandStringArgument.h"
#include "Core/Commands/CommandFlagArgument.h"

#include "Core/Platform/Process.h"

namespace MicroBuild {

BuildDocsCommand::BuildDocsCommand(IPluginInterface* pluginInterface)
	: m_pluginInterface(pluginInterface)
{
	SetName("builddocs");
	SetShortName("bd");
	SetDescription("Builds documentation for the given project file.");

	CommandPathArgument* workspaceArg = new CommandPathArgument();
	workspaceArg->SetName("Workspace File Path");
	workspaceArg->SetShortName("w");
	workspaceArg->SetDescription("Path to workspace file to build documentation for.");
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
	projectArg->SetDescription("Name of project in the workspace to build documentation for.");
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

bool BuildDocsCommand::Invoke(CommandLineParser* parser)
{
	MB_UNUSED_PARAMETER(parser);

	Log(LogSeverity::Warning, "Generating documentation ...\n");

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

	ProjectFile& config = state->GetConfigFile();

	// The files used for generating documentation are a concatination of files from every single project within
	// this workspace. TODO: this may cause issues with platform-specific/macro-expanded file paths?
	std::vector<Platform::Path> files;

	std::vector<std::shared_ptr<IProject>> projects = workspace->GetProjects();
	for (auto& subProject : projects)
	{
		std::shared_ptr<IProjectState> subState = subProject->GetState(m_configuration, m_platform);
		if (!subState)
		{
			continue;
		}

		ProjectFile& subConfig = subState->GetConfigFile();
		if (!subConfig.GetCastedValue<bool>("Plugin.DocGeneration", "IncludeInDocumentation", false))
		{
			continue;
		}

		std::vector<Platform::Path> subFiles = subConfig.Get_Files_File();
		for (auto& file : subFiles)
		{
			files.push_back(file);
		}
	}

	// Grab metadata configuration data.
	Platform::Path tempDir = config.GetCastedValue<Platform::Path>("Plugin.DocGeneration", "TempDirectory", "");
	Platform::Path outputDir = config.GetCastedValue<Platform::Path>("Plugin.DocGeneration", "OutputDirectory", "");
	Platform::Path rootDir = config.GetCastedValue<Platform::Path>("Plugin.DocGeneration", "RootDirectory", "");
	Platform::Path doxyBinary = config.GetCastedValue<Platform::Path>("Plugin.DocGeneration", "DoxygenBinary", "");
	Platform::Path doxyTemplate = config.GetCastedValue<Platform::Path>("Plugin.DocGeneration", "DoxygenTemplate", "");

	if (!rootDir.IsDirectory())
	{
		Log(LogSeverity::Fatal, "Root directory for documentation generation does not exist: %s\n", rootDir.ToString().c_str());
		return false;
	}
	if (!tempDir.IsDirectory() && !tempDir.CreateAsDirectory())
	{
		Log(LogSeverity::Fatal, "Temporary directory for documentation generation does not exist: %s\n", tempDir.ToString().c_str());
		return false;
	}
	if (!outputDir.IsDirectory() && !outputDir.CreateAsDirectory())
	{
		Log(LogSeverity::Fatal, "Output directory for documentation generation does not exist: %s\n", outputDir.ToString().c_str());
		return false;
	}
	if (!doxyBinary.Exists())
	{
		Log(LogSeverity::Fatal, "Doxygen binary for documentation generation does not exist: %s\n", doxyBinary.ToString().c_str());
		return false;
	}
	if (!doxyTemplate.Exists())
	{
		Log(LogSeverity::Fatal, "Doxygen template for documentation generation does not exist: %s\n", doxyTemplate.ToString().c_str());
		return false;
	}

	// Get a list of files we care about.
	std::vector<std::string> headerFiles;

	for (auto& file : files)
	{
		if (file.GetExtension() == "h")
		{
			headerFiles.push_back(file.ToString());
		}
	}

	// Create doxygen file.
	Platform::Path templatePath = tempDir.AppendFragment("doxygen.config", true);

	std::string doxyTemplateSource;
	if (!Strings::ReadFile(doxyTemplate, doxyTemplateSource))
	{
		Log(LogSeverity::Fatal, "Failed to read doxygen template for documentation generation: %s\n", doxyTemplate.ToString().c_str());
		return false;
	}

	typedef std::pair<std::string, std::string> Tag;
	std::vector<Tag> tags;

	tags.push_back(Tag("$(Project.Name)",				config.GetCastedValue<std::string>("Project", "Name", "")));
	tags.push_back(Tag("$(Project.Files)",				Strings::Join(headerFiles, " ")));
	tags.push_back(Tag("$(Template.Path)",				doxyTemplate.GetDirectory().ToString().c_str()));

	for (auto& tag : tags)
	{
		doxyTemplateSource = Strings::Replace(doxyTemplateSource, tag.first, tag.second);
	}

	if (!Strings::WriteFile(templatePath, doxyTemplateSource))
	{
		Log(LogSeverity::Fatal, "Failed to write doxygen template for documentation generation: %s\n", templatePath.ToString().c_str());
		return false;
	}

	// Doxygen arguments.
	std::vector<std::string> arguments;
	arguments.push_back(templatePath.ToString());

	// Generate all the actions required to build metadata.
	bool bBuildFailed = false;
	auto startTime = std::chrono::high_resolution_clock::now();

	Platform::Process process;
	if (process.Open(doxyBinary, outputDir, arguments, false))
	{
		process.Wait();

		int exitCode = process.GetExitCode();
		if (exitCode != 0)
		{
			Log(LogSeverity::Fatal, "doxygen failed with exit code %i.\n", exitCode);
			return false;
		}
	}
	else
	{
		Log(LogSeverity::Fatal, "Failed to start msbuild process.\n");
		return false;
	}
	
	auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
	auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();
	Log(LogSeverity::Info, "Documentation generated, built in %.1f seconds\n", elapsedMs / 1000.0f);

	return !bBuildFailed;
}

}; // namespace MicroBuild