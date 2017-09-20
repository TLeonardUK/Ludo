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

#include "Visitor/ClangAstVisitor.h"
#include "Visitor/ClangMetaTagParser.h"

#include "Metadata/MetadataAttribute.h"
#include "Metadata/MetadataModule.h"
#include "Metadata/MetadataClass.h"
#include "Metadata/MetadataMethod.h"
#include "Metadata/MetadataField.h"

#include "Core/Helpers/Strings.h"

#include "Helpers/Clang.h"

#include <stdarg.h>

#define CX_OPT_PRINT_DEBUG_LOG 0

namespace MicroBuild {

ClangAstVisitor::ClangAstVisitor(Platform::Path sourcePath, Platform::Path generatedPath, CXTranslationUnit translationUnit)
	: m_translationUnit(translationUnit)
	, m_sourcePath(sourcePath)
	, m_generatedPath(generatedPath)
{
}

bool ClangAstVisitor::ExtractMetatdata(MetadataModule& module)
{
	CXCursor TranslationCursor = clang_getTranslationUnitCursor(m_translationUnit);	
	
	// Extract all metadata.
	m_bOperationFailed = false;
	m_metadataModule = &module;
	clang_visitChildren(TranslationCursor, VisitNodeThunk, this);

	// Now we have all the metadata objects collected, do some
	// semantic analysis to ensure its all valid, during this pass
	// we also build up our type list.
	for (size_t i = 0; i < module.Classes.size(); i++)
	{
		MetadataClass::Ptr object = module.Classes[i];
		if (!AnalyseClass(object))
		{
			return false;
		}
	}
	for (size_t i = 0; i < module.Enums.size(); i++)
	{
		MetadataEnum::Ptr object = module.Enums[i];
		if (!AnalyseEnum(object))
		{
			return false;
		}
	}

	return !m_bOperationFailed;
}

CXChildVisitResult ClangAstVisitor::VisitNodeThunk(CXCursor cursor, CXCursor parent, CXClientData clientData)
{
	MB_UNUSED_PARAMETER(parent);

	ClangAstVisitor* visitor = reinterpret_cast<ClangAstVisitor*>(clientData);
	return visitor->VisitNode(cursor);
}

CXChildVisitResult ClangAstVisitor::VisitNode(CXCursor cursor)
{
	CXCursorKind kind = clang_getCursorKind(cursor);

	// If we are failing, abort.
	if (m_bOperationFailed)
	{
		return CXChildVisit_Break;
	}

	// Normal node.
	switch (kind)
	{
	case CXCursor_MacroExpansion:
		{
			if (GetCursorName(cursor) == "META" ||
				GetCursorName(cursor) == "GENERATED_BODY")
			{
				return VisitAttribute(cursor);
			}

			return CXChildVisit_Continue;
		}
	case CXCursor_ClassTemplate:
	//case CXCursor_ClassTemplatePartialSpecialization:
	case CXCursor_StructDecl:
	case CXCursor_ClassDecl:
		{
			// Skip forward-declarations.
			if (!clang_isCursorDefinition(cursor))
			{
				return CXChildVisit_Continue;
			}

			return VisitClass(cursor);
		}
	case CXCursor_CXXBaseSpecifier:
		{
			return VisitBaseSpecifier(cursor);
		}
	case CXCursor_EnumDecl:
		{
			// Skip forward-declarations.
			if (!clang_isCursorDefinition(cursor))
			{
				return CXChildVisit_Continue;
			}

			return VisitEnum(cursor);
		}
	case CXCursor_EnumConstantDecl:
		{
			return VisitEnumKey(cursor);
		}
	case CXCursor_VarDecl:
	case CXCursor_FieldDecl:
		{
			return VisitField(cursor);
		}
	case CXCursor_Constructor:
	case CXCursor_Destructor:
	case CXCursor_CXXMethod:
		{
			return VisitMethod(cursor);
		}
	case CXCursor_Namespace:
		{
			return CXChildVisit_Recurse;
		}
	default:
		{
			return CXChildVisit_Continue;
		}
	}
}

CXChildVisitResult ClangAstVisitor::VisitAttribute(CXCursor cursor)
{
	CXSourceRange unexpandedRange = clang_getCursorExtent(cursor);

	if (GetCursorName(cursor) == "META")
	{
		std::vector<MetadataAttribute> attributes;

		ClangMetaTagParser parser;
		if (parser.Parse(this, unexpandedRange, cursor, attributes))
		{
			CXSourceLocation attributeLocation = clang_getRangeEnd(unexpandedRange);

			// Attach attributes to cursor directly after our token.
			for (auto& attribute : attributes)
			{
				attribute.Location = attributeLocation;
				m_attributes.push_back(attribute);
			}

			// If there are no attributes, we add a dummy one just to mark that 
			// we have actually had a meta tag attached to the symbol.
			if (attributes.size() == 0)
			{
				MetadataAttribute dummy;
				dummy.Location = attributeLocation;
				dummy.Name = "MetadataTag";
				m_attributes.push_back(dummy);
			}

			return CXChildVisit_Continue;
		}
		else
		{
			return CXChildVisit_Break;
		}
	}
	else if (GetCursorName(cursor) == "GENERATED_BODY")
	{
		m_generatedBodyTags.push_back(unexpandedRange);
	}

	return CXChildVisit_Continue;
}

std::vector<MetadataAttribute> ClangAstVisitor::GetAttributesForCursor(CXCursor cursor)
{
	std::vector<MetadataAttribute> result;

	unsigned int currentStartOffset = 0;	
	CXSourceLocation currentStart = clang_getCursorLocation(cursor);
	CXFile currentFile;

	clang_getSpellingLocation(currentStart, &currentFile, nullptr, nullptr, &currentStartOffset);

	for (size_t i = 0; i < m_attributes.size(); i++)
	{
		MetadataAttribute& attribute = m_attributes[i];

		unsigned int attrCharOffset = 0;
		CXFile attrFile;
		clang_getSpellingLocation(attribute.Location, &attrFile, nullptr, nullptr, &attrCharOffset);

		if (attrFile == currentFile && attrCharOffset < currentStartOffset)
		{
			result.push_back(attribute);
			m_attributes.erase(m_attributes.begin() + i);
			i--;
		}
	}

	return result;
}

CXChildVisitResult ClangAstVisitor::VisitClass(CXCursor cursor)
{
	// No attributes? No a class we care about then.
	std::vector<MetadataAttribute> attributes = GetAttributesForCursor(cursor);
	if (attributes.size() <= 0)
	{
		return CXChildVisit_Continue;
	}

	// Construct an object representing this class and register it within he module.
	MetadataClass::Ptr instance = std::make_shared<MetadataClass>();
	instance->Comment = CXStringToString(clang_Cursor_getRawCommentText(cursor));
	instance->Attributes = attributes;
	instance->ClangType = clang_getCursorType(cursor);
	instance->ClangCursor = cursor;
	instance->Name = CXStringToString(clang_getCursorSpelling(cursor));
	instance->FullyQualifiedName = GetCursorFullyQualifiedName(cursor);
	instance->MangledName = CXStringToString(clang_getCursorUSR(cursor));
	instance->SanitizedName = GetCursorSanitizedName(cursor);
	instance->AccessQualifier = GetCursorAccessQualifier(cursor);
	instance->Location = GetCursorLocation(cursor);
	instance->IsAbstract = false;
	instance->InMainFile = IsCursorInTargetFile(cursor);
	instance->IsStruct = (clang_getCursorKind(cursor) == CXCursor_StructDecl);
	instance->bContainsGeneratedBodyTag = DoesCursorContainBodyTag(cursor, instance->GeneratedBodyLine);

	if (clang_getCursorKind(cursor) == CXCursor_ClassTemplate)
	{
		instance->IsTemplated = true;
		instance->IsTemplateInstance = false;

		std::vector<MetadataAttribute> specializationAttrs = m_attributes;

		unsigned specializationNum = clang_getNumTemplateSpecialization(cursor);

		for (unsigned i = 0; i < specializationNum; i++)
		{
			CXCursor classCursor = clang_getTemplateSpecialization(cursor, i);

			std::string baseName = GetCursorFullyQualifiedName(classCursor);
#if CX_OPT_PRINT_DEBUG_LOG
			Log(LogSeverity::Info, "Specialization[%u]: '%s'.\n", i, baseName.c_str());
#endif

			// Construct an object representing this class and register it within he module.
			MetadataClass::Ptr specInstance = std::make_shared<MetadataClass>();
			specInstance->Attributes = instance->Attributes;
			specInstance->ClangType = clang_getCursorType(classCursor);
			specInstance->ClangCursor = cursor;
			specInstance->Name = CXStringToString(clang_getCursorSpelling(classCursor));
			specInstance->FullyQualifiedName = GetCursorFullyQualifiedName(classCursor);
			specInstance->SanitizedName = GetCursorSanitizedName(classCursor);
			specInstance->MangledName = CXStringToString(clang_getCursorUSR(classCursor));
			specInstance->AccessQualifier = GetCursorAccessQualifier(classCursor);
			specInstance->Location = GetCursorLocation(classCursor);
			specInstance->IsAbstract = false;
			specInstance->InMainFile = IsCursorInTargetFile(classCursor);
			specInstance->IsTemplated = true;
			specInstance->IsTemplateInstance = true;
			specInstance->TemplateBaseClass = instance;
			specInstance->IsStruct = instance->IsStruct;
			specInstance->bContainsGeneratedBodyTag = instance->bContainsGeneratedBodyTag;

			for (int j = 0; j < clang_Type_getNumTemplateArguments(specInstance->ClangType); j++)
			{
				CXType argType = clang_Type_getTemplateArgumentAsType(specInstance->ClangType, j);
				CXTemplateArgumentKind kind = clang_Type_getTemplateArgumentKind(specInstance->ClangType, j);

				MetadataTemplateType templateType;
				templateType.ClangType = argType;

				switch (kind)
				{
				case CXTemplateArgumentKind_Type:
				{
					templateType.BaseType = EMetadataTemplateBaseType::Type;
					break;
				}
				case CXTemplateArgumentKind_Integral:
				{
					templateType.BaseType = EMetadataTemplateBaseType::Integer;
					templateType.Value = clang_Type_getTemplateArgumentValue(specInstance->ClangType, j);
					break;
				}
				default:
				{
					return Error(classCursor, LogSeverity::Fatal, "Metadata template types can only accept type or integral arguments.");
				}
				}

				specInstance->TemplateTypes.push_back(templateType);
			}
			 
			m_metadataModule->Classes.push_back(specInstance);

			// Visit the children of the unspecialized template, we will swap template 
			// arguments of all class members as we encounter them.
			clang_visitChildren(classCursor, VisitNodeThunk, this);

			// If we need to visit again, reset attributes so next pass also extracts them. 
			// This is butt ugly, this needs handling better.
			if (i < specializationNum - 1)
			{
				m_attributes = specializationAttrs;
			}

			if (m_bOperationFailed)
			{
				return CXChildVisit_Break;
			}
		}
	}
	else
	{
		instance->IsTemplated = false;
		instance->IsTemplateInstance = false;
	}

	m_metadataModule->Classes.push_back(instance);

	return CXChildVisit_Recurse;
}

bool ClangAstVisitor::GenerateForwardDeclaredClasses(CXCursor cursor)
{
	// Construct an object representing this class and register it within he module.
	MetadataClass::Ptr instance = std::make_shared<MetadataClass>();
	instance->ClangType = clang_getCursorType(cursor);
	instance->ClangCursor = cursor;
	instance->Name = CXStringToString(clang_getCursorSpelling(cursor));
	instance->FullyQualifiedName = GetCursorFullyQualifiedName(cursor);
	instance->SanitizedName = GetCursorSanitizedName(cursor);
	instance->MangledName = CXStringToString(clang_getCursorUSR(cursor));
	instance->AccessQualifier = GetCursorAccessQualifier(cursor);
	instance->Location = GetCursorLocation(cursor);
	instance->IsAbstract = false;
	instance->InMainFile = false;
	instance->IsStruct = (clang_getCursorKind(cursor) == CXCursor_StructDecl);
	instance->bContainsGeneratedBodyTag = true;

	if (clang_getCursorKind(cursor) == CXCursor_ClassTemplate)
	{
		instance->IsTemplated = true;
		instance->IsTemplateInstance = false;

		unsigned specializationNum = clang_getNumTemplateSpecialization(cursor);

		for (unsigned i = 0; i < specializationNum; i++)
		{
			CXCursor classCursor = clang_getTemplateSpecialization(cursor, i);

			std::string baseName = GetCursorFullyQualifiedName(classCursor);
#if CX_OPT_PRINT_DEBUG_LOG
			Log(LogSeverity::Info, "Specialization[%u]: '%s'.\n", i, baseName.c_str());
#endif

			// Construct an object representing this class and register it within he module.
			MetadataClass::Ptr specInstance = std::make_shared<MetadataClass>();
			specInstance->ClangType = clang_getCursorType(classCursor);
			specInstance->ClangCursor = cursor;
			specInstance->Name = CXStringToString(clang_getCursorSpelling(classCursor));
			specInstance->FullyQualifiedName = GetCursorFullyQualifiedName(classCursor);
			specInstance->SanitizedName = GetCursorSanitizedName(classCursor);
			specInstance->MangledName = CXStringToString(clang_getCursorUSR(classCursor));
			specInstance->AccessQualifier = GetCursorAccessQualifier(classCursor);
			specInstance->Location = GetCursorLocation(classCursor);
			specInstance->IsAbstract = false;
			specInstance->InMainFile = false;
			specInstance->IsTemplated = true;
			specInstance->IsTemplateInstance = true;
			specInstance->TemplateBaseClass = instance;
			specInstance->IsStruct = instance->IsStruct;
			specInstance->bContainsGeneratedBodyTag = true;

			for (int j = 0; j < clang_Type_getNumTemplateArguments(specInstance->ClangType); j++)
			{
				CXType argType = clang_Type_getTemplateArgumentAsType(specInstance->ClangType, j);
				CXTemplateArgumentKind kind = clang_Type_getTemplateArgumentKind(specInstance->ClangType, j);

				MetadataTemplateType templateType;
				templateType.ClangType = argType;

				switch (kind)
				{
				case CXTemplateArgumentKind_Type:
				{
					templateType.BaseType = EMetadataTemplateBaseType::Type;
					break;
				}
				case CXTemplateArgumentKind_Integral:
				{
					templateType.BaseType = EMetadataTemplateBaseType::Integer;
					templateType.Value = clang_Type_getTemplateArgumentValue(specInstance->ClangType, j);
					break;
				}
				default:
				{
					Error(classCursor, LogSeverity::Fatal, "Metadata template types can only accept type or integral arguments.");
					return false;
				}
				}

				specInstance->TemplateTypes.push_back(templateType);
			}

			m_metadataModule->Classes.push_back(specInstance);

			// Visit the children of the unspecialized template, we will swap template 
			// arguments of all class members as we encounter them.
			clang_visitChildren(classCursor, VisitNodeThunk, this);

			if (m_bOperationFailed)
			{
				return false;
			}
		}
	}
	else
	{
		instance->IsTemplated = false;
		instance->IsTemplateInstance = false;
	}

	m_metadataModule->Classes.push_back(instance);

	return true;
}

CXChildVisitResult ClangAstVisitor::VisitBaseSpecifier(CXCursor cursor)
{
	MetadataClass::Ptr scope = m_metadataModule->Classes.back();
    assert(scope);

	MetadataBaseClass baseClass;
	baseClass.ClangType = clang_getCursorType(clang_getTypeDeclaration(clang_getCursorType(cursor)));
	if (baseClass.ClangType.kind == CXType_Invalid)
	{
		baseClass.ClangType = clang_getCursorType(cursor);
	}

	scope->BaseClasses.push_back(baseClass);

	return CXChildVisit_Continue;
}

CXChildVisitResult ClangAstVisitor::VisitEnum(CXCursor cursor)
{
	// No attributes? No a class we care about then.
	std::vector<MetadataAttribute> attributes = GetAttributesForCursor(cursor);
	if (attributes.size() <= 0)
	{
		return CXChildVisit_Continue;
	}

	// Construct an object representing this class and register it within he module.
	MetadataEnum::Ptr instance		= std::make_shared<MetadataEnum>();
	instance->Comment				= CXStringToString(clang_Cursor_getRawCommentText(cursor));
	instance->Attributes			= attributes;
	instance->ClangType				= clang_getCursorType(cursor);
	instance->Name					= CXStringToString(clang_getCursorSpelling(cursor));
	instance->FullyQualifiedName	= GetCursorFullyQualifiedName(cursor);
	instance->SanitizedName			= GetCursorSanitizedName(cursor);
	instance->MangledName			= CXStringToString(clang_getCursorUSR(cursor));
	instance->AccessQualifier		= GetCursorAccessQualifier(cursor);
	instance->Location				= GetCursorLocation(cursor);
	instance->InMainFile			= IsCursorInTargetFile(cursor);

	m_metadataModule->Enums.push_back(instance);

	return CXChildVisit_Recurse;
}

CXChildVisitResult ClangAstVisitor::VisitEnumKey(CXCursor cursor)
{
	MetadataEnum::Ptr scope = m_metadataModule->Enums.back();
    assert(scope);

	// Construct an object representing this class and register it within he module.
	MetadataEnumKey::Ptr instance	= std::make_shared<MetadataEnumKey>();
	instance->Comment				= CXStringToString(clang_Cursor_getRawCommentText(cursor));
	instance->Attributes			= GetAttributesForCursor(cursor);
	instance->Name					= CXStringToString(clang_getCursorSpelling(cursor));
	instance->FullyQualifiedName	= GetCursorFullyQualifiedName(cursor);
	instance->SanitizedName			= GetCursorSanitizedName(cursor);
	instance->MangledName			= CXStringToString(clang_getCursorUSR(cursor));
	instance->AccessQualifier		= GetCursorAccessQualifier(cursor);
	instance->Location				= GetCursorLocation(cursor);
	instance->Constant				= clang_getEnumConstantDeclValue(cursor);

	scope->Keys.push_back(instance);

	return CXChildVisit_Continue;
}

CXChildVisitResult ClangAstVisitor::VisitMethod(CXCursor cursor)
{
	// Do not care about globals.
	if (m_metadataModule->Classes.size() <= 0)
	{
		return CXChildVisit_Continue;
	}

	// Varidic methods are not supported.
	if (clang_Cursor_isVariadic(cursor) || clang_getCursorKind(cursor) == CXCursor_FunctionTemplate)
	{
		return Error(cursor, LogSeverity::Fatal, "Template and varidic methods and functions are not supported by the metdata system.");
	}

	MetadataClass::Ptr scope = m_metadataModule->Classes.back();

	// No attributes? No a class we care about then.
	std::vector<MetadataAttribute> attributes = GetAttributesForCursor(cursor);
	if (attributes.size() <= 0)
	{
		return CXChildVisit_Continue;
	}

	// Construct an object representing this class and register it within the scope.
	MetadataMethod::Ptr instance = std::make_shared<MetadataMethod>();
	instance->Attributes = attributes;
	instance->Comment = CXStringToString(clang_Cursor_getRawCommentText(cursor));
	instance->Name = CXStringToString(clang_getCursorSpelling(cursor));
	instance->FullyQualifiedName = GetCursorFullyQualifiedName(cursor);
	instance->SanitizedName = GetCursorSanitizedName(cursor);
	instance->MangledName = CXStringToString(clang_getCursorUSR(cursor));
	instance->AccessQualifier = GetCursorAccessQualifier(cursor);
	instance->Location = GetCursorLocation(cursor);
	instance->IsConstructor =
		!!clang_CXXConstructor_isDefaultConstructor(cursor) ||
		!!clang_CXXConstructor_isConvertingConstructor(cursor) ||
		!!clang_CXXConstructor_isCopyConstructor(cursor) ||
		!!clang_CXXConstructor_isMoveConstructor(cursor);
	instance->IsStatic = !!clang_CXXMethod_isStatic(cursor);
	instance->IsConst = !!clang_CXXMethod_isConst(cursor);
	instance->IsVirtual = !!clang_CXXMethod_isVirtual(cursor);
	instance->IsAbstract = !!clang_CXXMethod_isPureVirtual(cursor);
	instance->ClangReturnType = clang_getResultType(clang_getCursorType(cursor));
	instance->ClangCursor = cursor;

	if (instance->IsAbstract)
	{
		scope->IsAbstract = true;
	}

	// Extract parameter info.
	int parameterCount = clang_Cursor_getNumArguments(cursor);
	for (int i = 0; i < parameterCount; i++)
	{
		CXCursor parameterCursor = clang_Cursor_getArgument(cursor, i);

		MetadataParameter::Ptr parameter = std::make_shared<MetadataParameter>();
		parameter->Attributes = GetAttributesForCursor(parameterCursor);
		instance->Comment = CXStringToString(clang_Cursor_getRawCommentText(cursor));
		parameter->Name = CXStringToString(clang_getCursorSpelling(parameterCursor));
		parameter->FullyQualifiedName = GetCursorFullyQualifiedName(parameterCursor);
		parameter->SanitizedName = GetCursorSanitizedName(parameterCursor);
		parameter->MangledName = CXStringToString(clang_getCursorUSR(parameterCursor));
		parameter->AccessQualifier = GetCursorAccessQualifier(parameterCursor);
		parameter->Location = GetCursorLocation(parameterCursor);
		parameter->ClangType = clang_getCursorType(parameterCursor);

		instance->Parameters.push_back(parameter);
	}

	scope->Methods.push_back(instance);

	return CXChildVisit_Continue;
}

CXChildVisitResult ClangAstVisitor::VisitField(CXCursor cursor)
{
	// Do not care about globals.
	if (m_metadataModule->Classes.size() <= 0)
	{
		return CXChildVisit_Continue;
	}

	MetadataClass::Ptr scope = m_metadataModule->Classes.back();

	// No attributes? No a class we care about then.
	std::vector<MetadataAttribute> attributes = GetAttributesForCursor(cursor);
	if (attributes.size() <= 0)
	{
		return CXChildVisit_Continue;
	}

	// Construct an object representing this class and register it within the scope.
	MetadataField::Ptr instance = std::make_shared<MetadataField>();
	instance->Attributes = attributes;
	instance->Comment = CXStringToString(clang_Cursor_getRawCommentText(cursor));
	instance->Name = CXStringToString(clang_getCursorSpelling(cursor));
	instance->FullyQualifiedName = GetCursorFullyQualifiedName(cursor);
	instance->SanitizedName = GetCursorSanitizedName(cursor);
	instance->MangledName = CXStringToString(clang_getCursorUSR(cursor));
	instance->AccessQualifier = GetCursorAccessQualifier(cursor);
	instance->Location = GetCursorLocation(cursor);
	instance->IsStatic = (clang_Cursor_getStorageClass(cursor) == CX_SC_Static);
	instance->ClangType = clang_getCursorType(cursor);
	instance->ClangCursor = cursor;
	 
	scope->Fields.push_back(instance);

	return CXChildVisit_Continue;
}

bool ClangAstVisitor::AnalyseClass(MetadataClass::Ptr object)
{
#if CX_OPT_PRINT_DEBUG_LOG
	Log(LogSeverity::Warning, "Class: %s\n", object->FullyQualifiedName.c_str());
#endif

	std::string baseName = CXStringToString(clang_getTypeSpelling(object->ClangType));
#if CX_OPT_PRINT_DEBUG_LOG
	Log(LogSeverity::Info, "Type Name: %s\n", baseName.c_str());
#endif
	
	// Resolve template types.
	if (!object->IsTemplated || object->IsTemplateInstance)
	{
		for (auto& type : object->TemplateTypes)
		{
			if (type.BaseType == EMetadataTemplateBaseType::Type)
			{
				if (!ResolveType(object->ClangCursor, object, type.ClangType, type.Type))
				{
					Error(object->ClangCursor, LogSeverity::Fatal, "Failed to resolve metadata for template argument type '%s' of class '%s'. Are you sure its been defined as a metadata object?",
						CXStringToString(clang_getTypeSpelling(type.ClangType)).c_str(),
						object->FullyQualifiedName.c_str()
					);
					return false;
				}
			}
		}
	}

	// Resolve base class types.
	for (auto& type : object->BaseClasses)
	{
		if (!ResolveClass(object->ClangCursor, object, type.ClangType, type.Class))
		{
			Error(object->ClangCursor, LogSeverity::Fatal, "Failed to find metadata for base class '%s' of class '%s'. Are you sure its been defined as a metadata object?", 
				CXStringToString(clang_getTypeSpelling(type.ClangType)).c_str(),
				object->FullyQualifiedName.c_str()
			);
			return false;
		}

#if CX_OPT_PRINT_DEBUG_LOG
		Log(LogSeverity::Warning, "\tBase: %s\n", type.Class->FullyQualifiedName.c_str());
#endif
	}

	// If we are not in main file, no need to resolve any of this.
	if (!object->InMainFile)
	{
		return true;
	}

	// If we are an unspecialized template, ignore, nothing else valid to analyze.
	if (object->IsTemplated && !object->IsTemplateInstance)
	{
		return true;
	}

	// Resolve fields.
	for (auto field : object->Fields)
	{
		if (!AnalyseField(field, object))
		{
			return false;
		}

#if CX_OPT_PRINT_DEBUG_LOG
		Log(LogSeverity::Warning, "\tField: %s\n", field->FullyQualifiedName.c_str());
#endif
	}

	// Resolve methods.
	for (auto method : object->Methods)
	{
		if (!AnalyseMethod(method, object))
		{
			return false;
		}

#if CX_OPT_PRINT_DEBUG_LOG
		Log(LogSeverity::Warning, "\tMethod: %s\n", method->FullyQualifiedName.c_str());
#endif
	}

	// If we are a class, ensure we are derived from Object.
	MetadataClass::Ptr baseClass = m_metadataModule->FindClassByName("Ludo::Object");
	if (object->IsStruct)
	{
		if (IsDerivedFrom(object, baseClass))
		{
			Error(object->ClangCursor, LogSeverity::Fatal, "Metadata struct '%s' derives from Object. Only metadata classes should derive from the Object base class.",
				object->FullyQualifiedName.c_str()
			);
			return false;
		}
	}
	else
	{
		if (!IsDerivedFrom(object, baseClass))
		{
			Error(object->ClangCursor, LogSeverity::Fatal, "Metadata class '%s' does not derive from Object. All metadata classes should derive from the Object base class.",
				object->FullyQualifiedName.c_str()
			);
			return false;
		}
	}

	// If we are a class, ensure we contain GENERATED_BODY macro, 
	// and that its the first thing in the class body.
	if (!object->bContainsGeneratedBodyTag)
	{
		Error(object->ClangCursor, LogSeverity::Fatal, "Metadata class or struct '%s' does not contain GENERATED_BODY() tag, this should be the first item in your class.",
			object->FullyQualifiedName.c_str()
		);
		return false;
	}	

	return true;
}

bool ClangAstVisitor::AnalyseEnum(MetadataEnum::Ptr object)
{
	// If we are not in main file, no need to resolve any of this.
	if (object->InMainFile)
	{
		return true;
	}

#if CX_OPT_PRINT_DEBUG_LOG
	Log(LogSeverity::Warning, "Enum: %s\n", object->FullyQualifiedName.c_str());
#endif

	// Nothing we really need to valid here?

	return true;
}

bool ClangAstVisitor::AnalyseMethod(MetadataMethod::Ptr object, MetadataClass::Ptr scope)
{
	if (!ResolveType(object->ClangCursor, scope, object->ClangReturnType, object->ReturnType))
	{
		/*Error(object->ClangCursor, LogSeverity::Fatal, "Failed to find metadata for type '%s' of class method '%s'. Are you sure it is a primitive type, or has been defined as a metadata object?", 
			CXStringToString(clang_getTypeSpelling(object->ClangReturnType)).c_str(), 
			object->FullyQualifiedName.c_str()
		);*/
		return false;
	}

	for (auto parameter : object->Parameters)
	{
		if (!ResolveType(object->ClangCursor, scope, parameter->ClangType, parameter->Type))
		{
			/*Error(object->ClangCursor, LogSeverity::Fatal, "Failed to find metadata for type '%s' of class method '%s' parameter '%s'. Are you sure it is a primitive type, or has been defined as a metadata object?", 
				CXStringToString(clang_getTypeSpelling(object->ClangReturnType)).c_str(), 
				object->FullyQualifiedName.c_str(), 
				parameter->FullyQualifiedName.c_str()
			);*/
			return false;
		}
	}

	return true;
}

bool ClangAstVisitor::AnalyseField(MetadataField::Ptr object, MetadataClass::Ptr scope)
{
	if (!ResolveType(object->ClangCursor, scope, object->ClangType, object->Type))
	{
		/*Error(object->ClangCursor, LogSeverity::Fatal, "Failed to find metadata for type '%s' of class field '%s'. Are you sure it is a primitive type, or has been defined as a metadata object?",
			CXStringToString(clang_getTypeSpelling(object->ClangType)).c_str(),
			object->FullyQualifiedName.c_str()
		);*/
		return false;
	}

	return true;
}

bool ClangAstVisitor::ResolveClass(CXCursor cursor, MetadataClass::Ptr scope, CXType type, MetadataClass::Ptr& OutValue)
{
	MetadataType::Ptr baseType;

	if (!ResolveType(cursor, scope, type, baseType) || 
		baseType->PrimitiveType != MetadataPrimitiveType::Class)
	{
		return false;
	}

	OutValue = baseType->Class;

	return true;
}

bool ClangAstVisitor::ResolveType(CXCursor cursor, MetadataClass::Ptr scope, CXType type, MetadataType::Ptr& outValue)
{
	// Reject qualifiers we do not support.
	if (clang_isVolatileQualifiedType(type) ||
		clang_isRestrictQualifiedType(type))
	{
		Error(cursor, LogSeverity::Fatal, "Type is invalid, metadata cannot be applied to volatile or restrict qualified types.");
		return false;
	}

	// Reject arrays, we only support container usage.
	if (clang_getNumElements(type) >= 0)
	{
		Error(cursor, LogSeverity::Fatal, "Type is invalid, metadata cannot be applied to raw array types, use containers instead.");
		return false;
	}

	// Do we have type type defined somewhere already?
	outValue = m_metadataModule->FindTypeByType(type);
	if (!outValue)
	{
		// Create a new type instance to represent this clang type.
		MetadataType::Ptr instance = std::make_shared<MetadataType>();
		instance->ClangType = type;
		instance->IsConst = !!clang_isConstQualifiedType(type);

		if (type.kind != CXType_SubstTemplateTypeParm)
		{
			outValue = instance;
			m_metadataModule->Types.push_back(instance);
		}

		switch (type.kind)
		{
			// --------------------------------------------------------------------
			// Primitive Types
			// --------------------------------------------------------------------

			case CXType_Void:
			{
				instance->PrimitiveType = MetadataPrimitiveType::Void;
				break;
			}
			case CXType_Bool:
			{
				instance->PrimitiveType = MetadataPrimitiveType::Bool;
				break;
			}
			case CXType_UChar:
			{
				instance->PrimitiveType = MetadataPrimitiveType::UInt8;
				break;
			}
			case CXType_Char_U:
				// Fallthrough
			case CXType_Char_S:
			{
				instance->PrimitiveType = MetadataPrimitiveType::Int8;
				break;
			}
			case CXType_Short:
				// Fallthrough
			case CXType_Char16:
			{
				instance->PrimitiveType = MetadataPrimitiveType::Int16;
				break;
			}
			case CXType_Long:
				// Fallthrough
			case CXType_Int:
				// Fallthrough
			case CXType_Char32:
			{
				instance->PrimitiveType = MetadataPrimitiveType::Int32;
				break;
			}
			case CXType_UShort:
			{
				instance->PrimitiveType = MetadataPrimitiveType::UInt16;
				break;
			}
			case CXType_ULong:
				// Fallthrough
			case CXType_UInt:
			{
				instance->PrimitiveType = MetadataPrimitiveType::UInt32;
				break;
			}
			case CXType_ULongLong:
			{
				instance->PrimitiveType = MetadataPrimitiveType::UInt64;
				break;
			}
			case CXType_LongLong:
			{
				instance->PrimitiveType = MetadataPrimitiveType::Int64;
				break;
			}
			case CXType_Float:
			{
				instance->PrimitiveType = MetadataPrimitiveType::Float;
				break;
			}
			case CXType_Double:
			{
				instance->PrimitiveType = MetadataPrimitiveType::Double;
				break;
			}

			// --------------------------------------------------------------------
			// Alias Types
			// --------------------------------------------------------------------
			case CXType_Enum:
			{
				instance->PrimitiveType = MetadataPrimitiveType::Enum;

				CXType baseType = clang_getCursorType(clang_getTypeDeclaration(type));
				if (baseType.kind == CXType_Invalid)
				{
					baseType = type;
				}

				instance->Enum = m_metadataModule->FindEnumByType(baseType);

				if (!instance->Enum)
				{
					Error(cursor, LogSeverity::Fatal, "Failed to find metadata for enum '%s'. Are you sure its been defined as a metadata object?",
						CXStringToString(clang_getTypeSpelling(baseType)).c_str()
					);
					return false;
				}
				break;
			}

			// --------------------------------------------------------------------
			// Reference Types
			// --------------------------------------------------------------------
			case CXType_Pointer:
			{
				instance->PrimitiveType = MetadataPrimitiveType::Pointer;
				if (!ResolveType(cursor, scope, clang_getPointeeType(type), instance->PointerType))
				{
					return false;
				}
				break;
			}

			case CXType_LValueReference:
			{
				instance->PrimitiveType = MetadataPrimitiveType::LValueReference;
				if (!ResolveType(cursor, scope, clang_getPointeeType(type), instance->PointerType))
				{
					return false;
				}
				break;
			}

			case CXType_RValueReference:
			{
				instance->PrimitiveType = MetadataPrimitiveType::RValueReference;
				if (!ResolveType(cursor, scope, clang_getPointeeType(type), instance->PointerType))
				{
					return false;
				}
				break;
			}

			case CXType_Record:
			case CXType_Elaborated:
			{
				instance->PrimitiveType = MetadataPrimitiveType::Class;
				
				CXCursor decl = clang_getTypeDeclaration(type);

				// If this is a forward-declared class, we need to create a dummy class to represent it.
				// This is fairly bleh, but its the best solution available.
				if (!clang_isCursorDefinition(decl))
				{
					if (!m_metadataModule->FindClassByCursor(decl))
					{
						std::string str = GetCursorFullyQualifiedName(decl);

						CXCursor baseDecl = decl;
						if (clang_Type_getNumTemplateArguments(type) > 0)
						{
							baseDecl = clang_getSpecializedCursorTemplate(decl);
						}

						if (!GenerateForwardDeclaredClasses(baseDecl))
						{
							return false;
						}
					}
				}

				CXType baseType = clang_getCursorType(decl);
				if (baseType.kind == CXType_Invalid)
				{
					baseType = type;
				}

				instance->Class = m_metadataModule->FindClassByType(baseType);				
				if (!instance->Class)
				{
					Error(cursor, LogSeverity::Fatal, "Failed to find metadata for class '%s'. Are you sure its been defined as a metadata object?",
						CXStringToString(clang_getTypeSpelling(baseType)).c_str()
					);
					return false;
				}

				break;
			}  
			
			case CXType_TemplateSpecialization:
			{
				instance->PrimitiveType = MetadataPrimitiveType::Class;

				CXCursor decl = clang_getTypeDeclaration(type);
				if (decl.kind == CXCursor_ClassTemplate)
				{
					instance->Class = m_metadataModule->FindUnspecializedTemplateClassByCursor(decl);
					if (!instance->Class)
					{
						return false;
					}
				}
				else
				{
					CXType declType = clang_getCursorType(decl);
					
					if (!ResolveClass(cursor, scope, declType, instance->Class))
					{
						return false;
					}
				}

				break;
			}

			/*
			case CXType_TemplateTypeParm:
			{
				if (scope != nullptr)
				{
					return false;
				}

				int Index = clang_Type_getTemplateTypeParamIndex(type);
				MB_UNUSED_PARAMETER(Index);
				LD_ASSERT(Index >= 0 && Index < static_cast<int>(scope->TemplateTypes.size()));
			
				outValue = scope->TemplateTypes[Index].Type;

				break;
			}
			*/

			case CXType_SubstTemplateTypeParm:
			{
				CXType replacementType = clang_Type_getSubstTemplateTypeReplacement(type);

				if (!ResolveType(cursor, scope, replacementType, outValue))
				{
					return false;
				}

				break;
			}

			default:
			{
				Error(cursor, LogSeverity::Fatal, "Unsupported data type or format for metadata: '%s'.",
					CXStringToString(clang_getTypeSpelling(type)).c_str()
				);
				return false;
			}
		}

		return true;
	}

	return true;
}

bool ClangAstVisitor::IsDerivedFrom(MetadataClass::Ptr subject, MetadataClass::Ptr baseClass)
{
	if (subject == baseClass)
	{
		return true;
	}

	for (auto& base : subject->BaseClasses)
	{
		if (IsDerivedFrom(base.Class, baseClass))
		{
			return true;
		}
	}

	return false;
}

bool ClangAstVisitor::DoesCursorContainBodyTag(CXCursor cursor, int& lineOutput)
{
	CXSourceRange range = clang_getCursorExtent(cursor);
	CXSourceLocation rangeStart = clang_getRangeStart(range);
	CXSourceLocation rangeEnd = clang_getRangeEnd(range);

	unsigned int rangeStartOffset = 0;
	unsigned int rangeEndOffset = 0;
	CXFile rangeFile; 

	clang_getSpellingLocation(rangeStart, &rangeFile, nullptr, nullptr, &rangeStartOffset);
	clang_getSpellingLocation(rangeEnd, nullptr, nullptr, nullptr, &rangeEndOffset);

	for (auto& bodyTag : m_generatedBodyTags)
	{
		CXSourceLocation bodyTagLocation = clang_getRangeStart(bodyTag);
		unsigned int bodyTagStart;
		unsigned int bodyTagLine;

		clang_getSpellingLocation(bodyTagLocation, nullptr, &bodyTagLine, nullptr, &bodyTagStart);

		if (bodyTagStart >= rangeStartOffset && bodyTagStart <= rangeEndOffset)
		{
			lineOutput = bodyTagLine;
			return true;
		}
	}

	return false;
}

bool ClangAstVisitor::IsCursorInTargetFile(CXCursor cursor)
{
	Platform::Path filename;
	Clang::GetCursorLocation(cursor, &filename, nullptr);
	return filename == m_sourcePath;
}

std::string ClangAstVisitor::GetCursorName(CXCursor cursor)
{
	CXString cxName = clang_getCursorSpelling(cursor);
	std::string name = clang_getCString(cxName);
	clang_disposeString(cxName);
	return name;
}

std::string ClangAstVisitor::GetTokenName(CXToken cursor)
{
	CXString cxName = clang_getTokenSpelling(m_translationUnit, cursor);
	std::string name = clang_getCString(cxName);
	clang_disposeString(cxName);
	return name;
}

std::string ClangAstVisitor::CXStringToString(CXString string)
{
	const char* cval = clang_getCString(string);

	if (cval == nullptr)
	{
		return "";
	}

	std::string name = cval;
	clang_disposeString(string);
	return name;
}

std::string ClangAstVisitor::GetCursorFullyQualifiedName(CXCursor cursor)
{
	std::string result = "";

	while (clang_Cursor_isNull(cursor) == false &&
		   clang_getCursorKind(cursor) != CXCursor_TranslationUnit)
	{
		CXString displayName = clang_getCursorDisplayName(cursor);
		std::string finalName = clang_getCString(displayName);

		clang_disposeString(displayName);

		if (clang_getCursorKind(cursor) == CXCursor_CXXMethod)
		{
			if (clang_CXXMethod_isConst(cursor))
			{
				finalName += " const";
			}
		}

		if (result.empty())
		{
			result = finalName;
		}
		else
		{
			result = Strings::Format("%s::%s", finalName.c_str(), result.c_str());
		}

		cursor = clang_getCursorSemanticParent(cursor);
	}

	return result;
}

std::string ClangAstVisitor::GetCursorSanitizedName(CXCursor cursor)
{
	std::string name = GetCursorFullyQualifiedName(cursor);
	for (size_t i = 0; i < name.size(); i++)
	{
		char& c = name.at(i);
		if ((c < '0' || c > '9') &&
			(c < 'A' || c > 'Z') &&
			(c < 'a' || c > 'z') &&
			c != '_')
		{
			c = '_';
		}
	}
	return "Metadata_" + name;
}

MetadataAccessQualifier ClangAstVisitor::GetCursorAccessQualifier(CXCursor cursor)
{
	CX_CXXAccessSpecifier accessSpecifier = clang_getCXXAccessSpecifier(cursor);
	switch (accessSpecifier)
	{
	case CX_CXXPublic:
		return MetadataAccessQualifier::Public;
	case CX_CXXPrivate:
		return MetadataAccessQualifier::Private;
	case CX_CXXProtected:
		return MetadataAccessQualifier::Protected;
	case CX_CXXInvalidAccessSpecifier:
		// Fallthrough
	default:
		// In this case we assume we are trying to get the access specifier for a top-level element, which
		// is for all intents and purposes permanently public.
		return MetadataAccessQualifier::Public;
	}
}

MetadataLocation ClangAstVisitor::GetCursorLocation(CXCursor cursor)
{
	CXSourceLocation clangLocation = clang_getCursorLocation(cursor);

	CXFile clangFileName;
	unsigned int line;
	unsigned int column;
	clang_getFileLocation(clangLocation, &clangFileName, &line, &column, nullptr);

	MetadataLocation location;
	location.FileName = CXStringToString(clang_getFileName(clangFileName));
	location.Line = static_cast<int>(line);
	location.Column = static_cast<int>(column);

	return location;
}

CXChildVisitResult ClangAstVisitor::Error(CXCursor source, LogSeverity level, const char* format, ...)
{
	m_bOperationFailed = true;

	va_list list;
	va_start(list, format);
	Clang::LogError(source, level, format, list);
	va_end(list);

	return CXChildVisit_Break;
}

CXTranslationUnit ClangAstVisitor::GetTranslationUnit()
{
	return m_translationUnit;
}

}; // namespace MicroBuild