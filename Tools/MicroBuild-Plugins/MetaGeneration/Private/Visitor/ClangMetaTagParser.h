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

#include "Visitor/ClangAstVisitor.h"

#include "Metadata/MetadataAttribute.h"

#include "clang-c/index.h"

namespace MicroBuild {

// Parse a meta tag in a source file into an attribute list.
// Example:
//	META(Name("Test"), Value(1))
// Turns into
//	Attribute 0: Name=NameAttribute Value="Test"
//	Attribute 1: Name=ValueAttribute Value=1
class ClangMetaTagParser
{
public:
	ClangMetaTagParser();
	
	// Does the actual parsing, returns true on success. 
	bool Parse(ClangAstVisitor* visitor, CXSourceRange range, CXCursor originCursor, std::vector<MetadataAttribute>& result);

protected:
	// Gets the literal value of the current token index.
	std::string GetTokenName();

	// Reads the next token in the stream, if it has the given kind (and optionally
	// the given spelling) the token stream will be progressed and it will return true.
	// If not, the token stream will not be progressed and it will return false.
	// An error message will also be printed on failure.
	bool ExpectToken(CXTokenKind kind, const char* spelling = nullptr, bool bPrintError = true);

	// Returns true if we have read every token in the tag.
	bool EndOfTokens();

private:
	ClangAstVisitor* m_visitor;
	CXSourceRange m_range;
	CXCursor m_originCursor;

	CXToken* m_tokens;
	unsigned int m_numTokens;
	
	unsigned int m_tokenIndex;

};

}; // namespace MicroBuild
