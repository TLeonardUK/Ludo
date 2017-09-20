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
#include "Metadata/MetadataAttribute.h"

#include "clang-c/index.h"

namespace MicroBuild {

// Visits each node in the ast of a clang translation unit and performs
// a specific action - usually extracting metadata.
class ClangAstVisitor
{
public:
	ClangAstVisitor(Platform::Path sourcePath, Platform::Path generatedPath, CXTranslationUnit translationUnit);
	
	// Visits each node and extracts and stores relevant metadata in the 
	// given module structure.
	bool ExtractMetatdata(MetadataModule& module);

public:

	// Checks a given symbol cursor is inside the target file (defined as the one the original 
	// translation unit passed to the constructor is contained in.
	bool IsCursorInTargetFile(CXCursor cursor);

	// Helper function, gets a cursors name without having to deal with the clang memory management.
	std::string GetCursorName(CXCursor cursor);

	// Helper function, gets a token name without having to deal with the clang memory management.
	std::string GetTokenName(CXToken token);

	// Converts a clang string into a stl string.
	std::string CXStringToString(CXString string);

	// Gets the fully qualified name of a cursor - including all scoping information.
	std::string GetCursorFullyQualifiedName(CXCursor cursor);

	// Gets the sanitized version of a cursor name. The sanitized name follows the identifier rules
	// of C++ so can be written directly into code.
	std::string GetCursorSanitizedName(CXCursor cursor);

	// Gets the access qualifier for the given cursor.
	MetadataAccessQualifier GetCursorAccessQualifier(CXCursor cursor);

	// Gets the location information for a given cursor.
	MetadataLocation GetCursorLocation(CXCursor cursor);

	// Gets all the META attributes defined next to the given cursor.
	std::vector<MetadataAttribute> GetAttributesForCursor(CXCursor cursor);

	// Attempts to find or create a metadata type for the given clang type. Returns true on success. 
	bool ResolveType(CXCursor cursor, MetadataClass::Ptr scope, CXType type, MetadataType::Ptr& OutValue);

	// Attempts to find or create a metadata class based on a clang type. Retusn true on success. 
	bool ResolveClass(CXCursor cursor, MetadataClass::Ptr scope, CXType type, MetadataClass::Ptr& OutValue);

	// Returns true if the given class derives from the other.
	bool IsDerivedFrom(MetadataClass::Ptr subject, MetadataClass::Ptr baseClass);

	// Returns true if the given cursors body contains a GENERATED_BODY tag.
	bool DoesCursorContainBodyTag(CXCursor cursor, int& lineOutput);

	// Writes out and error and fails the operation. Also returns the clang visitation result
	// to break out of visitation - so you an do something line return Error(...);
	CXChildVisitResult Error(CXCursor source, LogSeverity level, const char* format, ...);

	// Gets the translation unit being visited.
	CXTranslationUnit GetTranslationUnit();

protected:

	// Global callbacks for clang, routes each node visitation to the correct function below.
	CXChildVisitResult VisitNode(CXCursor cursor);
	static CXChildVisitResult VisitNodeThunk(CXCursor cursor, CXCursor parent, CXClientData clientData);

	// Called when visiting each symbol type we care about.
	CXChildVisitResult VisitAttribute(CXCursor cursor);
	CXChildVisitResult VisitClass(CXCursor cursor);
	CXChildVisitResult VisitEnum(CXCursor cursor);
	CXChildVisitResult VisitEnumKey(CXCursor cursor);
	CXChildVisitResult VisitBaseSpecifier(CXCursor cursor);
	CXChildVisitResult VisitMethod(CXCursor cursor);
	CXChildVisitResult VisitField(CXCursor cursor);

	// General helper functions.
	bool GenerateForwardDeclaredClasses(CXCursor cursor);

	// Called to do semantic analysis on the given objects.
	bool AnalyseClass(MetadataClass::Ptr object);
	bool AnalyseEnum(MetadataEnum::Ptr object);
	bool AnalyseMethod(MetadataMethod::Ptr object, MetadataClass::Ptr scope);
	bool AnalyseField(MetadataField::Ptr object, MetadataClass::Ptr scope);

private:
	Platform::Path m_sourcePath;
	Platform::Path m_generatedPath;
	CXTranslationUnit m_translationUnit;
	MetadataModule* m_metadataModule;
	bool m_bOperationFailed;

	std::vector<MetadataAttribute> m_attributes;

	std::vector<CXSourceRange> m_generatedBodyTags;

};

}; // namespace MicroBuild
