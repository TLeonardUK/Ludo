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

#include "PCH.h"

#include "Core/Public/Reflection/PrimitiveTypes/PrimitiveTypes.h"
#include "Core/Public/Reflection/TypeModule.h"

namespace Ludo {

#define DefinePrimitiveType(ClassName, NativeType) \
	class ClassName : public Type \
	{ \
	public: \
		ClassName() \
		{ \
			m_Name = StringId::Create(#NativeType); \
			m_FullName = StringId::Create(#NativeType); \
			m_UUID = StringId::Create(#NativeType); \
			m_MetaClassName = StringId::Create(#ClassName); \
			m_TypeInfoName = StringId::Create(typeid(NativeType).name()); \
			m_FileName = StringId::Create("n/a"); \
			m_Line = 0; \
		} \
	};

#include "Core/Private/Reflection/PrimitiveTypes/PrimitiveTypes.inc"

#undef DefinePrimitiveType

class TypeModule_Primitives : public TypeModule
{
public:
	TypeModule_Primitives()
	{
		RegisterModule(this);
	}

	virtual ~TypeModule_Primitives()
	{
		UnregisterModule(this);
	}

	virtual void RegisterTypes() override
	{
#define DefinePrimitiveType(ClassName, NativeType) Type::RegisterType<ClassName>();
#include "Core/Private/Reflection/PrimitiveTypes/PrimitiveTypes.inc"
#undef DefinePrimitiveType
	}

	virtual void UnregisterTypes() override
	{
#define DefinePrimitiveType(ClassName, NativeType) Type::UnregisterType<ClassName>();
#include "Core/Private/Reflection/PrimitiveTypes/PrimitiveTypes.inc"
#undef DefinePrimitiveType
	}

	virtual StringId GetName() const override
	{
		static StringId Name = StringId::Create("Primitives");
		return Name;
	}
};

void Register_Primitive_Metadata()
{
	static TypeModule_Primitives g_TypeModuleRegister_Primitives;
}

}; // namespace Ludo
