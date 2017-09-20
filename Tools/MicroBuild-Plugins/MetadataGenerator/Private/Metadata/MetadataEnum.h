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

#include "Metadata/MetadataObject.h"

#include <memory>

#include "clang-c/index.h"

namespace MicroBuild {

// Stores metadata specific to a given enum key.
struct MetadataEnumKey : 
	public MetadataObject
{
public:
	typedef std::shared_ptr<MetadataEnumKey> Ptr;

	// Constant value of this enum key.
	int64_t Constant;

};

// Stores metadata specific to a given enum.
struct MetadataEnum : 
	public MetadataObject
{
public:
	typedef std::shared_ptr<MetadataEnum> Ptr;

	// Clang representation of this enum.
	CXType ClangType;

	// All key values in the enumeration.
	std::vector<MetadataEnumKey::Ptr> Keys;

};

}; // namespace MicroBuild
