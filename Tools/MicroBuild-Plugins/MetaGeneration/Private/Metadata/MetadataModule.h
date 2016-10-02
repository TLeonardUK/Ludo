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

#include "Metadata/MetadataClass.h"
#include "Metadata/MetadataEnum.h"
#include "Metadata/MetadataObject.h"
#include "Metadata/MetadataType.h"

#include "clang-c/index.h"

namespace MicroBuild {

// Stores metadata specific to a given module (translation unit).
struct MetadataModule
	: public MetadataObject
{
public:
	std::vector<MetadataClass::Ptr> Classes;
	std::vector<MetadataEnum::Ptr>  Enums;
	std::vector<MetadataType::Ptr>  Types;

	// Tries to find class by the internal clang type.
	MetadataClass::Ptr FindClassByType(CXType type);

	// Tries to find a type by the internal clang type.
	MetadataType::Ptr FindTypeByType(CXType type);

	// Tries to find an enum by the internal clang type.
	MetadataEnum::Ptr FindEnumByType(CXType type);

};

}; // namespace MicroBuild
