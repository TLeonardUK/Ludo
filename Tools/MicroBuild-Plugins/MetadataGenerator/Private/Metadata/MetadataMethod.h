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
#include "Metadata/MetadataType.h"

#include <memory>

#include "clang-c/index.h"

namespace MicroBuild {
	
// Stores metadata specific to a parameter accepted by a method.
struct MetadataParameter : 
	public MetadataObject
{
public:
	typedef std::shared_ptr<MetadataParameter> Ptr;

	// Varidic value describing the default value 
	// of the parameter.
	// todo
	uint64_t DefaultValue;

	// Clang representation of the parameter type.
	CXType ClangType;

	// Datatype of this parameter.
	MetadataType::Ptr Type;

};

// Stores metadata specific to a given class method.
struct MetadataMethod : 
	public MetadataObject
{
public:
	typedef std::shared_ptr<MetadataMethod> Ptr;

	// Clang representation of position of this object in source code. 
	CXCursor ClangCursor;

	// True if is a constructor method.
	bool IsConstructor;

	// True if is a static method.
	bool IsStatic;

	// True if is a constant method.
	bool IsConst;

	// True if is a virtual method.
	bool IsVirtual;

	// True if is a pure-virtual method with no implementation.
	bool IsAbstract;

	// List of all parameters accepted by this method.
	std::vector<MetadataParameter::Ptr> Parameters;

	// Clang representation of the return type.
	CXType ClangReturnType;

	// Datatype this method returns.
	MetadataType::Ptr ReturnType;

};

}; // namespace MicroBuild
