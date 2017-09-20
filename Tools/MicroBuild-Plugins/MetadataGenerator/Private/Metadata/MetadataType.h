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
#include "Metadata/MetadataEnum.h"

#include <memory>

#include "clang-c/index.h"

namespace MicroBuild {

struct MetadataClass;

// Primitive type that a MetadataType actually represents.
enum class MetadataPrimitiveType
{
	Pointer,
	LValueReference,
	RValueReference,
	Class,
	Enum,

	Void,
	Bool,
	Int8,
	Int16,
	Int32,
	Int64,
	UInt8,
	UInt16,
	UInt32,
	UInt64,

	Float,
	Double
};

// Represents a type that can be passed around
struct MetadataType 
{
public:
	typedef std::shared_ptr<MetadataType> Ptr;

	// The clang type that represents us.
	CXType ClangType;

	// Class this type is pointing towards.
	std::shared_ptr<MetadataClass> Class;

	// Enum this type is pointing towards.
	std::shared_ptr<MetadataEnum> Enum;

	// If we are a pointer/reference, this is the type we are referncing.
	MetadataType::Ptr PointerType;

	// What type of compiler primitive type we are pointing to.
	MetadataPrimitiveType PrimitiveType;

	// True if we are const-qualified.
	bool IsConst;

	// Creates a human-readable representation of this type, this value is valid
	// to define directly in source code.
	std::string ToString();

};

}; // namespace MicroBuild
