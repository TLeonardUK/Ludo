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

#include "Metadata/MetadataFileInfo.h"

#include "Core/Config/ConfigFile.h"

namespace MicroBuild {

bool MetadataFileInfo::LoadManifest()
{
	ConfigFile file;

	std::vector<Platform::Path> includePaths;
	includePaths.push_back(ManifestPath.GetDirectory());

	if (!file.Parse(ManifestPath, includePaths))
	{
		return false;
	}

	file.Resolve();

	Hash = file.GetCastedValue<uint64_t>("Manifest", "Hash", 0);

	std::vector<std::pair<std::string, std::string>> pairs = file.GetPairs("Dependencies");
	for (auto& pair : pairs)
	{
		MetadataDependencyInfo dependency;
		dependency.SourcePath = pair.second;
		if (!StringCast<std::string, uint64_t>(pair.first, dependency.Hash))
		{
			continue;
		}
	}

	return true;
}

bool MetadataFileInfo::StoreManifest()
{
	ConfigFile file;
	file.SetOrAddValue("Manifest", "Hash", CastToString(Hash));

	for (MetadataDependencyInfo& dependency : Dependencies)
	{
		file.SetOrAddValue("Dependencies", CastToString(dependency.Hash), dependency.SourcePath.ToString());
	}

	return file.Serialize(ManifestPath);
}

uint64_t MetadataFileInfo::CalculateFileHash(const Platform::Path& path)
{
	uint64_t hash = 0;

	hash = path.GetModifiedTime();
	hash = Strings::Hash64(path.ToString(), hash);
	
	return hash;
}

std::vector<MetadataFileInfo> MetadataFileInfo::GetMultipleFileInfos(
	const std::vector<Platform::Path>& paths,
	std::vector<Platform::Path> includePaths,
	std::vector<std::string> defines,
	Platform::Path rootDirectory,
	Platform::Path outputDirectory
)
{
	static std::vector<std::string> validExtensions = { "h", "hpp", "hh" };

	std::vector<MetadataFileInfo> result;

	for (const Platform::Path& path : paths)
	{
		if (std::find(validExtensions.begin(), validExtensions.end(), path.GetExtension()) != validExtensions.end())
		{
			MetadataFileInfo info;
			info.SourcePath = path;

			Platform::Path relativePath = rootDirectory.RelativeTo(path);
			assert(relativePath.IsRelative());

			info.GeneratedPath	= outputDirectory + relativePath.ChangeExtension("generated." + relativePath.GetExtension());
			info.ManifestPath	= info.GeneratedPath.ChangeExtension("manifest");
			info.bOutOfDate		= false;
			info.Hash			= CalculateFileHash(info.GeneratedPath);
			info.CodePrefix		= "MD_" + CastToString(info.Hash) + "_";
			info.IncludePaths	= includePaths;
			info.Defines		= defines;

			Platform::Path baseDirectory = info.GeneratedPath.GetDirectory();
			if (!baseDirectory.Exists())
			{
				baseDirectory.CreateAsDirectory();
			}

			if (!info.ManifestPath.Exists() ||
				!info.GeneratedPath.Exists())
			{
				info.bOutOfDate = true;
			}
			else
			{
				uint64_t currentHash = info.Hash;

				if (!info.LoadManifest())
				{
					info.bOutOfDate = true;
				}
				else if (info.Hash != currentHash)
				{
					info.bOutOfDate = true;
					info.Hash = currentHash;
				}
				else
				{
					for (const MetadataDependencyInfo& dependencyInfo : info.Dependencies)
					{
						uint64_t dependencyHash = CalculateFileHash(dependencyInfo.SourcePath);

						if (!dependencyInfo.SourcePath.Exists() ||
							dependencyInfo.Hash != dependencyHash)
						{
							info.bOutOfDate = true;
							break;
						}
					}
				}
			}

			result.push_back(info);
		}
	}

	return result;
}

}; // namespace MicroBuild