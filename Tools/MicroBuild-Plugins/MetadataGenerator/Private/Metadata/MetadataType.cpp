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

#include "Core/Helpers/Strings.h"

#include "Metadata/MetadataClass.h"

namespace MicroBuild {

std::string MetadataType::ToString()
{
	std::string result;

	switch (PrimitiveType)
	{
	case MetadataPrimitiveType::Pointer:
		{
			result = PointerType->ToString() + "* ";
			break;
		}
	case MetadataPrimitiveType::LValueReference:
		{
			result = PointerType->ToString() + "& ";
			break;
		}
	case MetadataPrimitiveType::RValueReference:
		{
			result = PointerType->ToString() + "&& ";
			break;
		}
	case MetadataPrimitiveType::Class:
		{
			result = Class->FullyQualifiedName;
			/*
			if (Class->IsTemplated)
			{
				result += "<";
				for (size_t i = 0; i < Class->TemplateTypes.size(); i++)
				{
					MetadataTemplateType& templateType = Class->TemplateTypes[i];
					switch (templateType.BaseType)
					{
					case EMetadataTemplateBaseType::Integer:					
						{
							result += Strings::Format("%i", templateType.Value);
							break;
						}
					case EMetadataTemplateBaseType::Type:
						{
							result += templateType.Type->ToString();
							break;
						}
					}

					if (i < Class->TemplateTypes.size() - 1)
					{
						result += ", ";
					}
				}
				result += ">";
			}*/
			break;
		}
	case MetadataPrimitiveType::Enum:
		{
			result = Enum->FullyQualifiedName;
			break;
		}
	case MetadataPrimitiveType::Void:
		{
			result = "void";
			break;
		}
	case MetadataPrimitiveType::Bool:
		{
			result = "bool";
			break;
		}
	case MetadataPrimitiveType::Int8:
		{
			result = "signed char";
			break;
		}
	case MetadataPrimitiveType::Int16:
		{
			result = "signed short";
			break;
		}
	case MetadataPrimitiveType::Int32:
		{
			result = "signed int";
			break;
		}
	case MetadataPrimitiveType::Int64:
		{
			result = "signed long long";
			break;
		}
	case MetadataPrimitiveType::UInt8:
		{
			result = "unsigned char";
			break;
		}
	case MetadataPrimitiveType::UInt16:
		{
			result = "unsigned short";
			break;
		}
	case MetadataPrimitiveType::UInt32:
		{
			result = "unsigned int";
			break;
		}
	case MetadataPrimitiveType::UInt64:
		{
			result = "unsigned long long";
			break;
		}
	case MetadataPrimitiveType::Float:
		{
			result = "float";
			break;
		}
	case MetadataPrimitiveType::Double:
		{
			result = "double";
			break;
		}
	default:
		{
			assert(false);
		}
	}

	if (IsConst)
	{
		result += " const";
	}

	return result;
}

}; // namespace MicroBuild