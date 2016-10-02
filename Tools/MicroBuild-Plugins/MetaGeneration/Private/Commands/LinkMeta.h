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

#pragma once

#include "Core/Commands/Command.h"
#include "Core/Platform/Path.h"

namespace MicroBuild {

class IPluginInterface;

// Takes all metadata files built for a project and links them
// into a single project metadata file.
class LinkMetaCommand : public Command
{
public:
	LinkMetaCommand(IPluginInterface* pluginInterface);

protected:
	virtual bool Invoke(CommandLineParser* parser) override;

private:
	Platform::Path		m_workspaceFilePath;
	std::string			m_projectName;
	bool				m_rebuild;
	std::string			m_configuration;
	std::string			m_platform;
	IPluginInterface*	m_pluginInterface;

};

}; // namespace MicroBuild
