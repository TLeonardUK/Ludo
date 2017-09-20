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
#include "Metadata/MetadataField.h"
#include "Metadata/MetadataMethod.h"
#include "Metadata/MetadataType.h"

#include <memory>

#include "clang-c/index.h"

namespace MicroBuild {

// Stores metadata describing how a class inherits from
// a base class.
struct MetadataBaseClass
{
public:
	
	// The actual resolved class we are inheriting from.
	std::shared_ptr<MetadataClass> Class;

	// The clang type of the class we are inheriting from.
	CXType ClangType;

};

// Type of value a metadata template can contain.
enum class EMetadataTemplateBaseType
{
	Type,
	Integer
};

// Stores metadata describing a template argument.
struct MetadataTemplateType
{
public:

	// Type of template argument, can be a type or an integer.
	EMetadataTemplateBaseType BaseType;

	// Value if base-type is an integral argument.
	int64_t Value;

	// The actual resolved type of the template argument
	MetadataType::Ptr Type;

	// The clang type of the template argument.
	CXType ClangType;

};

// Stores metadata specific to a given class.
struct MetadataClass : 
	public MetadataObject
{
public:
	typedef std::shared_ptr<MetadataClass> Ptr;

	// Clang representation of position of this class in source code. May not
	// refer to the same type as ClangType (if its a template specialization it will
	// point to the base class). Its primarily used for error messages.
	CXCursor ClangCursor;

	// Clang representation of this class.
	CXType ClangType;

	// If true this class is declared using the struct keyword.
	bool IsStruct;

	// If true this class contains a GENERATED_BODY macro tag.
	bool bContainsGeneratedBodyTag;

	// The line the generated body tag was one, this is used to generate
	// a unique-id for the class.
	int GeneratedBodyLine;

	// If true this is a base template class without any of
	// the template arguments defined.
	bool IsTemplated;

	// If true this is an instantiated version of a template class..
	bool IsTemplateInstance;

	// If this is an instantiated version of a template this points to the base class.
	MetadataClass::Ptr TemplateBaseClass;

	// If this is an instantiated template, this contains the template arguments.
	std::vector<MetadataTemplateType> TemplateTypes;

	// True if class contains any pure virtual methods.
	bool IsAbstract;

	// Contains a list of direct ancestors of this class.
	std::vector<MetadataBaseClass> BaseClasses;

	// Contains a list of all fields in this class.
	std::vector<MetadataField::Ptr> Fields;

	// Contains a list of all methods in this class.
	std::vector<MetadataMethod::Ptr> Methods;

};

}; // namespace MicroBuild
