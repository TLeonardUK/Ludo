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

#include "Actions/IAction.h"
#include "Metadata/MetadataFileInfo.h"

namespace MicroBuild {

// Writes a metadata module for a given project which links all the 
// individual header files in the project.
class WriteProjectMetadataToFile : public IAction 
{
public:
	WriteProjectMetadataToFile(std::vector<MetadataFileInfo>& infos);

	virtual bool Execute() override;

private:
	std::vector<MetadataFileInfo>& m_fileInfos;

};

}; // namespace MicroBuild
