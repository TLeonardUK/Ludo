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

#include "Core/Public/Types/Array.h"
#include "Core/Public/Types/Map.h"
#include "Core/Public/Types/String.h"
#include "Core/Public/Types/StringId.h"

#include <typeindex>

namespace Ludo {

class Class;
class Attribute;
class Type;

/// \brief These structs are used in conjunction with TypeOf to determine 
///		   the type of an expression.

template <typename T>
struct TypeOfEvaluator
{
	static Type* Find(T x)
	{
		UNUSED_PARAMETER(x);
		return Type::FindByType<T>();
	}
};

template <typename T>
struct TypeOfEvaluator<T*>
{
	static Type* Find(T* x)
	{
		return Type::FindByTypeInfo(typeid(*x));
	}
};

/// \brief Handy operator that allows you to do typeof() to return the type of 
///		   a class or an expression.
#define TypeOf(...)				TypeOfEvaluator<std::remove_cv<decltype(__VA_ARGS__)>::type>::Find(x)
#define TypeOfStatic(...)		Type::FindByType<std::remove_cv<__VA_ARGS__>::type>()
#define TypeOfStringId(...)		Type::FindByName(__VA_ARGS__) 

/// \brief TODO
enum class TypeAccessQualifier
{
	Public,
	Private,
	Protected
};

/// \brief TODO
class Type
{
protected:
	friend class ReflectionFileScanner;

	static Array<Type*> m_Types;
	static Map<StringId, Type*> m_TypesByFullName;
	static Map<StringId, Type*> m_TypesByName;
	static Map<StringId, Type*> m_TypesByTypeInfo;
	static Map<StringId, Type*> m_TypesByMetaTypeInfo;

	Array<String> m_RawAttributes;
	Array<Attribute*> m_Attributes;
	StringId m_Name;
	StringId m_FullName;
	StringId m_UUID;
	StringId m_MetaClassName;
	StringId m_TypeInfoName;
	StringId m_FileName;
	int m_Line;

	TypeAccessQualifier m_AccessQualifier;

protected:
	virtual void OnPostRegistration();
	virtual void OnBuildClassTree();

public:
	Type();
	virtual ~Type();
	
	/// \brief TODO
	StringId GetName();

	/// \brief TODO
	StringId GetFullName();

	/// \brief TODO
	StringId GetUUID();

	/// \brief TODO
	Array<Attribute*> GetAttributes();

	/// \brief TODO
	template<typename Type>
	Type* FindAttribute()
	{
		for (Attribute* Attr : m_Attributes)
		{
			Type* Result = dynamic_cast<Type*>(Attr);
			if (Result != nullptr)
			{
				return Result;
			}
		}
		return nullptr;
	}

	/// \brief TODO
	TypeAccessQualifier GetAccessQualifier();

	/*
	//	virtual void* Clone(void* Other);

	virtual bool IsArray();
	virtual bool IsClass();
	virtual bool IsStruct();
	virtual bool IsField();
	virtual bool IsMethod();
	virtual bool IsEnum();
	virtual bool IsReference();
	virtual bool IsPointer();
	virtual bool IsConst();
	virtual bool IsStatic();

	virtual Type* GetDereferenceType();
	*/

public:
	//protected:

	/// \brief TODO
	template <typename DataType>
	static Type* FindByType()
	{
		static bool bLookedUp = false;
		static Type* Result = nullptr;

		if (!bLookedUp)
		{
			Result = FindByTypeInfo(typeid(DataType));
			bLookedUp = true;
		}

		return Result;
	}

	/// \brief TODO
	static Type* FindByName(const StringId& Name);

	/// \brief TODO
	static Type* FindByTypeInfo(const std::type_info& Id);

	/// \brief TODO
	static Array<Class*> GetDerivedClasses(Class* Base);

	/// \brief TODO
	template <typename DataType>
	static void RegisterType()
	{
		DataType* NewType = LD_ARENA_NEW(ReflectionMemoryArena, DataType);

#ifndef LD_SHIPPING_BUILD
		for (Type* OldType : m_Types)
		{
			Assert(OldType->m_UUID != NewType->m_UUID);
		}
#endif

		m_TypesByFullName.Set(NewType->m_FullName, NewType);
		m_TypesByName.Set(NewType->m_Name, NewType);
		m_TypesByTypeInfo.Set(NewType->m_TypeInfoName, NewType);
		m_TypesByMetaTypeInfo.Set(StringId::Create(typeid(DataType).name()), NewType);
		m_Types.Push(NewType);
	}

	/// \brief TODO
	template <typename DataType>
	static void UnregisterType()
	{
		Type* Instance = FindByTypeInfo(typeid(DataType));
		Assert(Instance != nullptr);

		m_TypesByFullName.RemoveValue(Instance);
		m_TypesByName.RemoveValue(Instance);
		m_TypesByTypeInfo.RemoveValue(Instance);
		m_TypesByMetaTypeInfo.RemoveValue(Instance);
		m_Types.Remove(Instance);

		LD_ARENA_DELETE(ReflectionMemoryArena, Instance);
	}

	/// \brief TODO
	static void FinalizeRegistration();

	/// \brief TODO
	static void Teardown();

};

}; // namespace Ludo
