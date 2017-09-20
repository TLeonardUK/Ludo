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

#include "Helpers/Clang.h"

#include "clang-c/index.h"

namespace MicroBuild {

ExtractMetadataFromHeader::ExtractMetadataFromHeader(MetadataFileInfo& info)
	: m_fileInfo(info)
{
	m_compileArgs.push_back("-Wmicrosoft");
	m_compileArgs.push_back("-Wunknown-pragmas");
	m_compileArgs.push_back("-Wno-pragma-once-outside-header");
	m_compileArgs.push_back("-DLD_REFLECTION_GENERATOR=1");
	m_compileArgs.push_back("-fms-compatibility");
	m_compileArgs.push_back("-xc++-header");
	m_compileArgs.push_back("-std=c++14"); // Rather not do this, but VS has a bunch of headers that require C++14.

	//m_compileArgs.push_back("-v");
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

bool ExtractMetadataFromHeader::WriteTranslationUnitErrors(CXTranslationUnit translationUnit)
{
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

	return bAnyFatalErrors;
}

bool ExtractMetadataFromHeader::ScanHeader(CXIndex context)
{
	// Attempt to generate an AST for the translation unit.
	CXTranslationUnit translationUnit;
	CXErrorCode code = clang_parseTranslationUnit2(
		context,
		m_fileInfo.SourcePath.ToString().c_str(),
		m_compileArgsData.data(),
		static_cast<int>(m_compileArgsData.size()),
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
	if (WriteTranslationUnitErrors(translationUnit))
	{
		return false;
	}

	// Walk the AST and attempt to extract whatever metadata information we require.
	ClangAstVisitor visitor(m_fileInfo.SourcePath, m_fileInfo.GeneratedHeaderPath, translationUnit);
	if (!visitor.ExtractMetatdata(m_fileInfo.Module))
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

			bool bAlreadyExists = false;

			for (auto& dep : action->m_fileInfo.Dependencies)
			{
				if (dep.SourcePath == dependency.SourcePath)
				{
					dep.Hash = dependency.Hash;
					bAlreadyExists = true;
					break;
				}
			}
			
			if (!bAlreadyExists)
			{
				action->m_fileInfo.Dependencies.push_back(dependency);
			}

			if (include_len == 1)
			{
				action->m_lastIncludedPath = dependency.SourcePath;
			}

			clang_disposeString(str);
		},
	this);

	// If we have any metadata types in this file, ensure we 
	// include the generated file as well.
	int mainFileTypes = 0;

	for (auto& pClass : m_fileInfo.Module.Classes)
	{
		if (pClass->InMainFile)
		{
			Log(LogSeverity::Warning, "Class: %s\n", pClass->FullyQualifiedName.c_str());
		}

		if (pClass->InMainFile)
		{
			mainFileTypes++;
		}
	}
	for (auto& pClass : m_fileInfo.Module.Enums)
	{
		if (pClass->InMainFile)
		{
			mainFileTypes++;
		}
	}

	if (mainFileTypes > 0)
	{
		if (m_lastIncludedPath != m_fileInfo.GeneratedHeaderPath)
		{
			Clang::LogError(clang_getTranslationUnitCursor(translationUnit), LogSeverity::Fatal, "File does not include generated header. Expected last file include to be for '%s'.",
				m_fileInfo.GeneratedHeaderPath.ToString().c_str()
			);
			return false;
		}
	}

	// Clean up.
	clang_disposeTranslationUnit(translationUnit);

	return true;
}

bool ExtractMetadataFromHeader::ScanSource(CXIndex context)
{
	// If we have no metadata in header file, no need to check source file.
	if (m_fileInfo.Module.Classes.size() == 0 &&
		m_fileInfo.Module.Enums.size() == 0 &&
		m_fileInfo.Module.Types.size() == 0)
	{
		return true;
	}

	// Attempt to generate an AST for the translation unit.
	CXTranslationUnit translationUnit;
	CXErrorCode code = clang_parseTranslationUnit2(
		context,
		m_fileInfo.SourceSourcePath.ToString().c_str(),
		m_compileArgsData.data(),
		static_cast<int>(m_compileArgsData.size()),
		0,
		0,
		CXTranslationUnit_SkipFunctionBodies,
		&translationUnit);

	if (code != CXError_Success)
	{
		Log(LogSeverity::Fatal, "Could not parse translation unit with error 0x%08x for '%s'.", code, m_fileInfo.SourceSourcePath.ToString().c_str());
		clang_disposeIndex(context);
		return false;
	}

	// Dump out any diagnostic messages that got emitted.
	if (WriteTranslationUnitErrors(translationUnit))
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

			if (include_len == 1)
			{
				CXString str = clang_getFileName(included_file);
				action->m_lastIncludedPath = clang_getCString(str);
				clang_disposeString(str);
			}
		},
	this);

	if (m_lastIncludedPath != m_fileInfo.GeneratedSourcePath)
	{
		Clang::LogError(clang_getTranslationUnitCursor(translationUnit), LogSeverity::Fatal, "File does not include generated source. Expected last file include to be for '%s'.",
			m_fileInfo.GeneratedSourcePath.ToString().c_str()
		);
		return false;
	}

	// Clean up.
	clang_disposeTranslationUnit(translationUnit);

	return true;
}

bool ExtractMetadataFromHeader::Execute() 
{
//	Log(LogSeverity::Info, "Scanning: %s\n", m_fileInfo.SourcePath.ToString().c_str());

	CXIndex context = clang_createIndex(0, 1);

	if (!ScanHeader(context))
	{
		return false;
	}

	/*if (!ScanSource(context))
	{
		return false;
	}*/

	clang_disposeIndex(context);

	return true;
}

}; // namespace MicroBuild