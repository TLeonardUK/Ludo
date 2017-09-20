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

#include "Metadata/MetadataModule.h"

namespace MicroBuild {

// Stores information on a dependency of a MetadataFileInfo
// structure.
struct MetadataDependencyInfo
{
public:
	Platform::Path	SourcePath;
	uint64_t		Hash;

};

// Stores information on an individual file that needs to 
// have meta data generated for it.
struct MetadataFileInfo 
{
public:

	// Directory the files came from.
	Platform::Path						RootDirectory;

	// Directory all generated files are written into.
	Platform::Path						OutputDirectory;

	// Source file of header the metadata is generated from.
	Platform::Path						SourcePath;

	// Source file of source the metadata is generated from.
	Platform::Path						SourceSourcePath;

	// The output .generated.h file the metadata will be written to.
	Platform::Path						GeneratedHeaderPath;

	// The output .generated.cpp file the metadata will be written to.
	Platform::Path						GeneratedSourcePath;

	// Manifest file used to keep track of incremental build state.
	Platform::Path						ManifestPath;

	// True if the source file needs its metadata regenerating.
	bool								bOutOfDate;

	// Prefix added to all code-identifiers for this files meta data.
	std::string							CodePrefix;

	// Include paths used when generating the meta data.
	std::vector<Platform::Path>			IncludePaths;

	// Defines used when generating the meta data.
	std::vector<std::string>			Defines;

	// The metadata module for the file, containing a representation of all
	// meta types within the file.
	MetadataModule						Module;

	// Hash which changes when the source file changes. Used to determine
	// when the file is out of date.
	uint64_t							Hash;

	// Keeps track of the hashes and paths to all other files the
	// source file is dependent on.
	std::vector<MetadataDependencyInfo>	Dependencies;

	// Loads hash and dependency data from the manifest file.
	bool LoadManifest();

	// Stores hash and dependency data into the manifest file.
	bool StoreManifest();

	// Calculates the state-hash for a given file. Used to figure
	// out of a file is stale and needs regenerating.
	static uint64_t CalculateFileHash(const Platform::Path& path);

	// Goes through a list of source files an generates an array of FileInfo
	// structures for them using the given properties.
	static std::vector<MetadataFileInfo> GetMultipleFileInfos(
		const std::vector<Platform::Path>& paths,
		std::vector<Platform::Path> includePaths,
		std::vector<std::string> defines,
		Platform::Path rootDirectory,
		Platform::Path outputDirectory,
		bool bDoNotCalculateOutOfDate = false
	);
};

}; // namespace MicroBuild
