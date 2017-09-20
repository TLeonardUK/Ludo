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

#include "Visitor/ClangMetaTagParser.h"

#include "Metadata/MetadataAttribute.h"

#include "Core/Helpers/Strings.h"

#include <stdarg.h>

namespace MicroBuild {

ClangMetaTagParser::ClangMetaTagParser()
{

}

bool ClangMetaTagParser::Parse(ClangAstVisitor* visitor, CXSourceRange range, CXCursor originCursor, std::vector<MetadataAttribute>& result)
{
	m_visitor = visitor;
	m_range = range;
	m_originCursor = originCursor;

	clang_tokenize(visitor->GetTranslationUnit(), range, &m_tokens, &m_numTokens);

	m_tokenIndex = 0;

	result.clear();

	// Open name and tag.
	if (!ExpectToken(CXToken_Identifier,  "META") ||
		!ExpectToken(CXToken_Punctuation, "("))
	{
		return false;
	}

	// Any values to read?
	if (!ExpectToken(CXToken_Punctuation, ")", false))
	{
		// Read each attribute that follows.
		while (!EndOfTokens())
		{
			// Read attribute identifier.
			if (!ExpectToken(CXToken_Identifier))
			{
				return false;
			}

			MetadataAttribute attribute;
			attribute.Name = GetTokenName();

			if (ExpectToken(CXToken_Punctuation, "(", false))
			{
				// Read each attribute parameter.
				while (!EndOfTokens())
				{
					if (!ExpectToken(CXToken_Literal, nullptr, false) &&
						!ExpectToken(CXToken_Identifier, nullptr, false))
					{
						ExpectToken(CXToken_Literal);
						return false;
					}

					attribute.Values.push_back(GetTokenName());

					// If the next token is not a comma, bail.
					if (!ExpectToken(CXToken_Punctuation, ",", false))
					{
						break;
					}
				}

				if (!ExpectToken(CXToken_Punctuation, ")"))
				{
					return false;
				}
			}

			result.push_back(attribute);

			// If the next token is not a comma, bail.
			if (!ExpectToken(CXToken_Punctuation, ",", false))
			{
				break;
			}
		}

		// Closing name and tag.
		if (!ExpectToken(CXToken_Punctuation, ")"))
		{
			return false;
		}
	}
	 
	clang_disposeTokens(visitor->GetTranslationUnit(), m_tokens, m_numTokens);

	return true;
}

std::string ClangMetaTagParser::GetTokenName()
{
	return m_visitor->GetTokenName(m_tokens[m_tokenIndex - 1]);
}

bool ClangMetaTagParser::ExpectToken(CXTokenKind kind, const char* spelling, bool bPrintError)
{
	CXToken token = m_tokens[m_tokenIndex];
	if (clang_getTokenKind(token) != kind ||
		(spelling != nullptr && m_visitor->GetTokenName(token) != spelling))
	{
		if (bPrintError)
		{
			std::string tokenId = "";
			if (spelling != nullptr)
			{
				tokenId = spelling;
			}
			else
			{
				switch (kind)
				{
				case CXToken_Identifier:
					{
						tokenId = "identifier";
						break;
					}
				case CXToken_Literal:
					{
						tokenId = "literal";
						break;
					}
				case CXToken_Keyword:
					{
						tokenId = "keyword";
						break;
					}
				case CXToken_Punctuation:
					{
						tokenId = "punctuation";
						break;
					}
				case CXToken_Comment:
					{
						tokenId = "comment";
						break;
					}
				default:
					{
						tokenId = "unknown";
						break;
					}
				}
			}

			m_visitor->Error(m_originCursor, LogSeverity::Fatal, "Malformed meta tag, expected token '%s'.", tokenId.c_str());
		}
		return false;
	}
	m_tokenIndex++;
	return true;
}


bool ClangMetaTagParser::EndOfTokens()
{
	return m_tokenIndex >= m_numTokens; 
}

}; // namespace MicroBuild