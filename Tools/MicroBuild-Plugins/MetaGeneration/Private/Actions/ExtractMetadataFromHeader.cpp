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

#include "Actions/ExtractMetadataFromHeader.h"
#include "Metadata/MetadataModule.h"
#include "Visitor/ClangAstVisitor.h"

#include "Core/Helpers/Strings.h"

#include "clang-c/index.h"

namespace MicroBuild {

ExtractMetadataFromHeader::ExtractMetadataFromHeader(MetadataFileInfo& info)
	: m_fileInfo(info)
{
	m_compileArgs.push_back("-Wmicrosoft");
	m_compileArgs.push_back("-Wunknown-pragmas");
	m_compileArgs.push_back("-Wno-pragma-once-outside-header");
	m_compileArgs.push_back("-DLD_REFLECTION_GENERATOR=1");
	m_compileArgs.push_back("-fms-compatibility-version=19.00.22609");
	m_compileArgs.push_back("-xc++-header");
	m_compileArgs.push_back("-std=c++14"); // Rather not do this, but VS has a bunch of headers that require C++14.

	for (std::string& define : m_fileInfo.Defines)
	{
		m_compileArgs.push_back(Strings::Format("-D%s", define.c_str()));
	}
	for (Platform::Path& path : m_fileInfo.IncludePaths)
	{
		m_compileArgs.push_back(Strings::Format("-I%s", path.ToString().c_str()));
	}
	for (std::string& arg : m_compileArgs)
	{
		m_compileArgsData.push_back(arg.c_str());
	}
}

bool ExtractMetadataFromHeader::Execute() 
{
	Log(LogSeverity::Info, "Scanning: %s\n", m_fileInfo.SourcePath.ToString().c_str());

	// Create a contex to compile within.
	CXIndex context = clang_createIndex(0, 1);

	// Attempt to generate an AST for the translation unit.
	CXTranslationUnit translationUnit;
	CXErrorCode code = clang_parseTranslationUnit2(
		context,
		m_fileInfo.SourcePath.ToString().c_str(),
		m_compileArgsData.data(),
		m_compileArgsData.size(),
		0,
		0,
		CXTranslationUnit_SkipFunctionBodies | CXTranslationUnit_DetailedPreprocessingRecord,
		&translationUnit);

	if (code != CXError_Success)
	{
		Log(LogSeverity::Fatal, "Could not parse translation unit with error 0x%08x for '%s'.", code, m_fileInfo.SourcePath.ToString().c_str());
		clang_disposeIndex(context);
		return false;
	}

	// Dump out any diagnostic messages that got emitted.
	bool bAnyFatalErrors = false;
	unsigned int diagnosticCount = clang_getNumDiagnostics(translationUnit);
	for (unsigned int i = 0; i < diagnosticCount; i++)
	{
		CXDiagnostic diagnostic = clang_getDiagnostic(translationUnit, i);
		CXString formatted = clang_formatDiagnostic(diagnostic, 0);

		CXDiagnosticSeverity severity = clang_getDiagnosticSeverity(diagnostic);
		if (severity == CXDiagnostic_Error ||
			severity == CXDiagnostic_Fatal)
		{
			bAnyFatalErrors = true;
		}

		std::string output = clang_getCString(formatted);

		CXFile sourceFile;
		unsigned int sourceLine;
		CXSourceLocation sourceLocation = clang_getDiagnosticLocation(diagnostic);
		clang_getFileLocation(sourceLocation, &sourceFile, &sourceLine, nullptr, nullptr);

		CXString sourceFileString = clang_getFileName(sourceFile);

		// Write out the error in a format visual studio will pick up.
		std::string finalValue = Strings::Format("%s(%u): %s", clang_getCString(sourceFileString), sourceLine, clang_getCString(formatted));
		Log(LogSeverity::Fatal, "%s\n", finalValue.c_str());

		clang_disposeString(formatted);
		clang_disposeDiagnostic(diagnostic);
	}

	if (bAnyFatalErrors)
	{
		return false;
	}

	// Walk the AST and attempt to extract whatever metadata information we require.
	ClangAstVisitor visitor(m_fileInfo.SourcePath, translationUnit);
	MetadataModule module;

	if (!visitor.ExtractMetatdata(module))
	{
		return false;
	}

	// Store dependencies in file info.
	clang_getInclusions(translationUnit, 
		[](
			CXFile included_file,
			CXSourceLocation* inclusion_stack,
			unsigned include_len,
			CXClientData client_data
		)
		{
				MB_UNUSED_PARAMETER(include_len);
				MB_UNUSED_PARAMETER(inclusion_stack);

				ExtractMetadataFromHeader* action = reinterpret_cast<ExtractMetadataFromHeader*>(client_data);

				CXString str = clang_getFileName(included_file);

				MetadataDependencyInfo dependency;
				dependency.SourcePath = clang_getCString(str);
				dependency.Hash = MetadataFileInfo::CalculateFileHash(dependency.SourcePath);

				action->m_fileInfo.Dependencies.push_back(dependency);

				clang_disposeString(str);
		},
	this);

	// Clean up.
	clang_disposeTranslationUnit(translationUnit);
	clang_disposeIndex(context);

	return true;
}

}; // namespace MicroBuild