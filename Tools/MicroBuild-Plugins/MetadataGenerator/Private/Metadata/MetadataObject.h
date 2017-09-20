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

#include "clang-c/index.h"

#include "Metadata/MetadataAttribute.h"

namespace MicroBuild {

// Defines the types of access qualifiers an object can have.
enum class MetadataAccessQualifier
{
	Public,
	Private,
	Protected
};

// Stores the source-code location for a given object.
struct MetadataLocation
{
	std::string FileName;
	int Line;
	int Column;
};

// Base class for all metadata containers.
struct MetadataObject
{
public:
	typedef std::shared_ptr<MetadataObject> Ptr;

	// Location information for the object.
	MetadataLocation Location;

	// All metadata attributes attached to the object using the
	// META() macro.
	std::vector<MetadataAttribute> Attributes;

	// Access qualifier that determines visibility from other semantic scopes.
	MetadataAccessQualifier AccessQualifier;

	// The name of the object, for something like a variable "int XYZ" it would
	// just be "XYZ".
	std::string Name;

	// The name of the object with all scoping information attached, a variable called
	// "int XYZ" in class "Class1" would be "Class1::XYZ".
	std::string FullyQualifiedName;

	// The mangled name of this object that is unique across translation unit boundries.
	std::string MangledName;

	// Sanitized version of the mangled name.
	std::string SanitizedName;

	// If true this object is defined in the main source code being parsed.
	bool InMainFile;

	// Comment that was declared above the metadata object.
	std::string Comment;

};

}; // namespace MicroBuild
