/*
Ludo Game Engine
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

namespace Ludo {

/// \brief TODO
class ReflectionFileScanner
{
private:
	CXTranslationUnit m_TranslationUnit;
	Path m_SourcePath;
	Path m_OutputPath;
	Path m_DependentsOutputPath;
	bool m_TreatAsPch;

	Array<String> m_ClangArguments;

	Array<Class*> m_Classes;
	Array<Field*> m_Fields;
	Array<Method*> m_Methods;
	Array<Enum*> m_Enums;

	Array<String> m_KnownClassNames;

	bool m_bFailedToCompile;

	Array<Class*> m_ClassStack;
	Array<Type*> m_SemanticStack;

	Array<String> m_PendingAttributes;

	Array<String> m_Dependencies;

	Array<CX_CXXAccessSpecifier> m_AccessSpecifierStack;

	String m_FileId;

private:
	static enum CXChildVisitResult VisitAttributesNodeThunk(CXCursor Cursor, CXCursor Parent, CXClientData ClientData);
	enum CXChildVisitResult VisitAttributesNode(CXCursor cursor, CXCursor parent);

	static enum CXChildVisitResult VisitGlobalNodeThunk(CXCursor Cursor, CXCursor Parent, CXClientData ClientData);
	enum CXChildVisitResult VisitGlobalNode(CXCursor cursor, CXCursor parent);

	static enum CXChildVisitResult VisitChildNodeThunk(CXCursor Cursor, CXCursor Parent, CXClientData ClientData);
	enum CXChildVisitResult VisitChildNode(CXCursor cursor, CXCursor parent);

	String GetCursorFullName(CXCursor Cursor);
	StringId GetCursorTypeString(CXType Type);
	StringId GenerateMetaClassName(const StringId& Id);
	void FillTypeLocation(Type* NewType, CXCursor Cursor);

	TypeAccessQualifier GetCurrentAccessQualifier();

	void VisitEnum(CXCursor Cursor);
	void VisitEnumKey(CXCursor Cursor);
	void VisitClass(CXCursor Cursor, bool bIsStruct);
	void VisitField(CXCursor Cursor);
	void VisitMethod(CXCursor Cursor, bool bConstructor);
	void VisitAttribute(CXCursor Cursor);

	void WriteError(CXCursor Source, const LogSeverity& Level, const String& Value);
	void WriteMethods(TextStream& Writer, const Array<Method*>& Methods);
	void WriteFields(TextStream& Writer, const Array<Field*>& Fields);
	void WriteAttributes(TextStream& Writer, const Array<String>& Attributes);
	void WriteDataType(TextStream& Writer, StringId TypeName, const String& FieldName);
	void WriteParameters(TextStream& Writer, const Array<Parameter>& Parameters);
	void WriteTypeInit(TextStream& Writer, Type* Value);

	Class* GetSemanticClass();

	StringId RemoveRef(const StringId& Id);

public:
	ReflectionFileScanner(
		const Path& SourceFile,
		const Path& OutputFile, 
		const Path& DependentsOutputPath,
		const String& FileId,
		const Array<String>& ClangArguments,
		bool bTreatAsPch
	);
	~ReflectionFileScanner();

	Error Scan();
	Error Save();

};

}; // namespace Ludo