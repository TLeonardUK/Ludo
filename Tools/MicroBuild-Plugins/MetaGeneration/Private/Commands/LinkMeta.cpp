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

#include "Commands/LinkMeta.h"

#include "Schemas/Plugin/PluginInterface.h"

#include "Core/Commands/CommandLineParser.h"
#include "Core/Commands/CommandPathArgument.h"
#include "Core/Commands/CommandStringArgument.h"
#include "Core/Commands/CommandFlagArgument.h"

namespace MicroBuild {

LinkMetaCommand::LinkMetaCommand(IPluginInterface* pluginInterface)
	: m_pluginInterface(pluginInterface)
{
	SetName("linkmeta");
	SetShortName("lm");
	SetDescription("Links all built metadata for the given project file.");

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

bool LinkMetaCommand::Invoke(CommandLineParser* parser)
{
	MB_UNUSED_PARAMETER(parser);

	Log(LogSeverity::Warning, "Linking metadata ...\n");

	return true;
}

}; // namespace MicroBuild