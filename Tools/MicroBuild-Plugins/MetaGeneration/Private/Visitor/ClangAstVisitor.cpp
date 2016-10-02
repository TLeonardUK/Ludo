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

#include <stdarg.h>

// parameter defaults
// check generated/uobject states of classes
// default types
// templated types for Array<x> Map<y> etc.
// figure out how to do Super::

namespace MicroBuild {

ClangAstVisitor::ClangAstVisitor(Platform::Path sourcePath, CXTranslationUnit translationUnit)
	: m_translationUnit(translationUnit)
	, m_sourcePath(sourcePath)
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
	for (MetadataClass::Ptr object : module.Classes)
	{
		if (!AnalyseClass(object))
		{
			return false;
		}
	}
	for (MetadataEnum::Ptr object : module.Enums)
	{
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
			if (GetCursorName(cursor) == "META")
			{
				return VisitAttribute(cursor);
			}

			return CXChildVisit_Continue;
		}
	case CXCursor_AnnotateAttr:
		{
			return VisitAttribute(cursor);
		}
	case CXCursor_ClassTemplate:
	case CXCursor_ClassTemplatePartialSpecialization:
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
	case CXCursor_FunctionTemplate:
	case CXCursor_Constructor:
	case CXCursor_Destructor:
	case CXCursor_CXXMethod:
	case CXCursor_FunctionDecl:
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
	instance->Comment				= CXStringToString(clang_Cursor_getRawCommentText(cursor));
	instance->Attributes			= attributes;
	instance->ClangType				= clang_getCursorType(cursor);
	instance->Name					= CXStringToString(clang_getCursorSpelling(cursor));
	instance->FullyQualifiedName	= GetCursorFullyQualifiedName(cursor);
	instance->MangledName			= CXStringToString(clang_getCursorUSR(cursor));
	instance->AccessQualifier		= GetCursorAccessQualifier(cursor);
	instance->Location				= GetCursorLocation(cursor);
	instance->IsAbstract			= false;
	instance->InMainFile			= IsCursorInTargetFile(cursor);

	if (clang_getCursorKind(cursor) == CXCursor_ClassTemplate ||
		clang_getCursorKind(cursor) == CXCursor_ClassTemplatePartialSpecialization)
	{
		instance->IsTemplated = true;
		instance->IsTemplateInstance = false;
	}
	else
	{
		instance->IsTemplated = false;
		instance->IsTemplateInstance = false;
	}

	m_metadataModule->Classes.push_back(instance);

	return CXChildVisit_Recurse;
}

CXChildVisitResult ClangAstVisitor::VisitBaseSpecifier(CXCursor cursor)
{
	MetadataClass::Ptr scope = m_metadataModule->Classes.back();
	assert(scope);

	MetadataBaseClass baseClass;
	baseClass.ClangType = clang_getCursorType(cursor);;

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
	instance->MangledName = CXStringToString(clang_getCursorUSR(cursor));
	instance->AccessQualifier = GetCursorAccessQualifier(cursor);
	instance->Location = GetCursorLocation(cursor);
	instance->IsStatic = !!clang_CXXMethod_isStatic(cursor);
	instance->IsConst = !!clang_CXXMethod_isConst(cursor);
	instance->IsVirtual = !!clang_CXXMethod_isVirtual(cursor);
	instance->IsAbstract = !!clang_CXXMethod_isPureVirtual(cursor);
	instance->ClangReturnType = clang_getResultType(clang_getCursorType(cursor));

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
	instance->MangledName = CXStringToString(clang_getCursorUSR(cursor));
	instance->AccessQualifier = GetCursorAccessQualifier(cursor);
	instance->Location = GetCursorLocation(cursor);
	instance->IsStatic = (clang_Cursor_getStorageClass(cursor) == CX_SC_Static);
	instance->ClangType = clang_getCursorType(cursor);

	scope->Fields.push_back(instance);

	return CXChildVisit_Continue;
}

bool ClangAstVisitor::AnalyseClass(MetadataClass::Ptr object)
{
	Log(LogSeverity::Warning, "Class: %s\n", object->FullyQualifiedName.c_str());

	// Resolve base class types.
	for (auto& type : object->BaseClasses)
	{
		if (!ResolveClass(type.ClangType, type.Class))
		{
			// todo: error
			return false;
		}

		Log(LogSeverity::Warning, "\tBase: %s\n", type.Class->FullyQualifiedName.c_str());
	}

	// Resolve template types.
	for (auto& type : object->TemplateTypes)
	{
		if (!ResolveType(type.ClangType, type.Type))
		{
			// todo: error
			return false;
		}
	}

	// Resolve fields.
	for (auto field : object->Fields)
	{
		if (!AnalyseField(field))
		{
			// todo: error
			return false;
		}

		Log(LogSeverity::Warning, "\tField: %s\n", field->FullyQualifiedName.c_str());
	}

	// Resolve methods.
	for (auto method : object->Methods)
	{
		if (!AnalyseMethod(method))
		{
			// todo: error
			return false;
		}

		Log(LogSeverity::Warning, "\tMethod: %s\n", method->FullyQualifiedName.c_str());
	}

	// If we are a class, ensure we are derived from Object.
	// todo

	// Ensure we contain GENERATED_BODY macro.
	// todo

	return true;
}

bool ClangAstVisitor::AnalyseEnum(MetadataEnum::Ptr object)
{
	Log(LogSeverity::Warning, "Enum: %s\n", object->FullyQualifiedName.c_str());

	// Nothing we really need to valid here?

	return true;
}

bool ClangAstVisitor::AnalyseMethod(MetadataMethod::Ptr object)
{
	if (!ResolveType(object->ClangReturnType, object->ReturnType))
	{
		// todo: error
		return false;
	}

	for (auto parameter : object->Parameters)
	{
		if (!ResolveType(parameter->ClangType, parameter->Type))
		{
			// todo: error
			return false;
		}
	}

	return true;
}

bool ClangAstVisitor::AnalyseField(MetadataField::Ptr object)
{
	if (!ResolveType(object->ClangType, object->Type))
	{
		// todo: error
		return false;
	}

	return true;
}

bool ClangAstVisitor::ResolveClass(CXType type, MetadataClass::Ptr& OutValue)
{
	// Do we have a reference to the class already?
	OutValue = m_metadataModule->FindClassByType(type);
	if (!OutValue)
	{
		// Nope, see if its a template class we have to instantiate.
		if (clang_Type_getNumTemplateArguments(type) > 0)
		{
			CXType classType = clang_Type_getClassType(type);
			OutValue = m_metadataModule->FindClassByType(type);
			if (OutValue)
			{
				CXCursor classCursor = clang_getTypeDeclaration(classType);

				// Construct an object representing this class and register it within he module.
				MetadataClass::Ptr instance = std::make_shared<MetadataClass>();
				instance->Attributes = OutValue->Attributes;
				instance->ClangType = type;
				instance->Name = CXStringToString(clang_getCursorSpelling(classCursor));
				instance->FullyQualifiedName = GetCursorFullyQualifiedName(classCursor);
				instance->MangledName = CXStringToString(clang_getCursorUSR(classCursor));
				instance->AccessQualifier = GetCursorAccessQualifier(classCursor);
				instance->Location = GetCursorLocation(classCursor);
				instance->IsAbstract = false;
				instance->InMainFile = IsCursorInTargetFile(classCursor);
				instance->IsTemplated = true;
				instance->IsTemplateInstance = true;
				instance->TemplateBaseClass = OutValue;

				for (int i = 0; i < clang_Type_getNumTemplateArguments(type); i++)
				{
					CXType argType = clang_Type_getTemplateArgumentAsType(type, i);
					
					MetadataTemplateType templateType;
					templateType.ClangType = argType;

					instance->TemplateTypes.push_back(templateType);
				}

				m_metadataModule->Classes.push_back(instance);

				OutValue = instance;
			}
		}

		return false;
	}

	return true;
}

bool ClangAstVisitor::ResolveType(CXType type, MetadataType::Ptr& outValue)
{
	// Reject qualifiers we do not support.
	if (clang_isVolatileQualifiedType(type) ||
		clang_isRestrictQualifiedType(type))
	{
		// todo: error
		return false;
	}

	// Reject arrays, we only support container usage.
	if (clang_getNumElements(type) >= 0)
	{
		// todo: error
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

		outValue = instance;
		m_metadataModule->Types.push_back(instance);

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

				instance->Enum = m_metadataModule->FindEnumByType(baseType);

				if (!instance->Enum)
				{
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
				if (!ResolveType(clang_getPointeeType(type), instance->PointerType))
				{
					return false;
				}
				break;
			}

			case CXType_LValueReference:
			{
				instance->PrimitiveType = MetadataPrimitiveType::LValueReference;
				if (!ResolveType(clang_getPointeeType(type), instance->PointerType))
				{
					return false;
				}
				break;
			}

			case CXType_RValueReference:
			{
				instance->PrimitiveType = MetadataPrimitiveType::RValueReference;
				if (!ResolveType(clang_getPointeeType(type), instance->PointerType))
				{
					return false;
				}
				break;
			}

			case CXType_Record:
			case CXType_Elaborated:
			{
				instance->PrimitiveType = MetadataPrimitiveType::Class;
				
				CXType baseType = clang_getCursorType(clang_getTypeDeclaration(type));
				
				if (!ResolveClass(baseType, instance->Class))
				{
					return false;
				}

				break;
			}

			case CXType_Unexposed:
			{
				CXType cannom = clang_Type_getClassType(type);

				int argCount = clang_Type_getNumTemplateArguments(cannom);
				if (argCount <= 0)
				{
					return false;
				}

				for (int i = 0; i < argCount; i++)
				{
					CXType argType = clang_Type_getTemplateArgumentAsType(cannom, i);
				}

				break;
			}

			default:
			{
				// todo: error: "Metadata types can only be primitives, metadata enums or metadata classes."
				return false;
			}
		}

		return true;
	}

	// reject volatile and restrict values.

	//clang_isConstQualifiedType
	//clang_getPointeeType
	//clang_getTypeDeclaration
	//clang_getNumElements
	//clang_Type_getClassType
	//clang_Type_getNumTemplateArguments
	//clang_Type_getTemplateArgumentAsType

	return true;
}

bool ClangAstVisitor::IsCursorInTargetFile(CXCursor cursor)
{
	Platform::Path filename;
	GetCursorLocation(cursor, &filename, nullptr);
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

void ClangAstVisitor::GetCursorLocation(CXCursor cursor, Platform::Path* filename, int* line)
{
	if (filename != nullptr)
	{
		*filename = "";
	}
	if (line != nullptr)
	{
		*line = 0;
	}

	CXFile locationFile;
	unsigned int unsignedLine;
	CXSourceLocation location = clang_getCursorLocation(cursor);
	clang_getSpellingLocation(location, &locationFile, &unsignedLine, nullptr, nullptr);

	if (locationFile == nullptr)
	{
		return;
	}

	if (filename != nullptr)
	{
		CXString locationFileString = clang_getFileName(locationFile);
		*filename = clang_getCString(locationFileString);
		clang_disposeString(locationFileString);
	}

	if (line != nullptr)
	{
		*line = static_cast<int>(unsignedLine);
	}
}

CXChildVisitResult ClangAstVisitor::Error(CXCursor source, LogSeverity level, const char* format, ...)
{
	m_bOperationFailed = true;

	va_list list;
	va_start(list, format);
	std::string formattedMessage = Strings::FormatVa(format, list);
	va_end(list);

	Platform::Path filename;
	int line;
	GetCursorLocation(source, &filename, &line);

	const char* levelName = "info";
	if (level == LogSeverity::Fatal)
	{
		levelName = "error";
	}
	else if (level == LogSeverity::Warning)
	{
		levelName = "warning";
	}

	std::string finalResult = Strings::Format("%s(%u): %s: %s", filename.ToString().c_str(), line, levelName, formattedMessage.c_str());

	// Make a normal long for the error.
	Log(level, "%s", finalResult.c_str());

	// And also print this out seperately so visual studio picks it up as a valid error.
	printf("%s\n", finalResult.c_str());

	return CXChildVisit_Break;
}

CXTranslationUnit ClangAstVisitor::GetTranslationUnit()
{
	return m_translationUnit;
}

}; // namespace MicroBuild