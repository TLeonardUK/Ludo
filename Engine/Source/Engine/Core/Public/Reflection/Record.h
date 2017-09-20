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

/*
\brief Represents a location within a source file that a given record is located at.
*/
struct RecordLocation
{
	StringId File;
	int Line;
	int Column;
};

/*
\brief Base class for all reflection records.

Reflection records are just an abstract idea, they are classes that represents
an individual component within the source code for the application, be it classes, methods
or other components.
*/
class Record
{
protected:
	static Array<Record*> m_Records;
	static Map<StringId, Record*> m_RecordsByFullName;
	static Map<StringId, Record*> m_RecordsByName;
	static Map<StringId, Record*> m_RecordsByMetaTypeInfo;

	Array<Attribute*> m_Attributes;
	StringId m_Name;
	StringId m_FullyQualifiedName;
	StringId m_MangledName;

	RecordLocation m_Location;

protected:

	// \brief Invoked after this record is registered, used to patch up references
	//		  that may not be valid prior to registration.
	virtual void OnPostRegistration();

	/*
	\brief Invoked after all records are registers, builds up the hierarchical record tree.

	The hierarchical record tree is primarily for Classes, it builds up lists of base/derived 
	classes based on their semantic parents.
	*/
	virtual void OnBuildTree();

public:

	// \brief Constructors.
	Record();

	// \brief Destructors.
	virtual ~Record();
	
	/*
	\brief		Gets the name of this record.
	\returns	The name of this record as a hashed string id.
	*/
	StringId GetName();

	/*
	\brief		Gets the fully qualified name of this record.
	\returns	The name of this record as a hashed string id.

	The fully qualified name is the name of the record along with namespace, template
	and other extra qualifiers. 

	e.g. 
	A record called 

		MyClass

	Could end up with a FQN similar to:

		Ludo::MyClass<float>
	*/
	StringId GetFullName();

	/*
	\brief		Gets the mangled name of this record.
	\returns	The mangled name of this record as a hashed string id.

	A mangled name is unique across all translation units in a compiled project. 
	It is not human-friendly and should never be shown to the user, its only use 
	is to uniquely identifier this record.
	*/
	StringId GetMangledName();

	/*
	\brief		Gets the location of this record in the source code.
	\returns	The location of this record in the source code.
	*/
	RecordLocation GetLocation();

	/*
	\brief		Gets a list of all attributes attached to this record.
	\returns	List of all attributes attached to this record.
	*/
	Array<Attribute*> GetAttributes();

	/*
	\brief		Finds the first attribute that is of the given type.	
	\tparam		AttrType	Type of attribute to look for.
	\returns	Pointer to attribute if found, otherwise nullptr if not found. If multiple 
				attributes of the type exist, the first defined is returned.
	*/
	template<typename AttrType>
	AttrType* FindAttribute()
	{
		for (Attribute* Attr : m_Attributes)
		{
			AttrType* Result = dynamic_cast<AttrType*>(Attr);
			if (Result != nullptr)
			{
				return Result;
			}
		}
		return nullptr;
	}

public:

	/*
	\brief		Gets the a record given its name.
	\param		Name Name of record to search for.
	\returns	Pointer to the record if it exists, otherwise nullptr.

	Will match both the name and fully qualified name of the record.
	*/
	static Record* FindByName(const StringId& Name);

	/*
	\brief		Gets the a record given its name, and casts it to the given type.
	\param		Name Name of record to search for.
	\returns	Pointer to the record if it exists, otherwise nullptr.

	Will match both the name and fully qualified name of the record.
	*/
	template <typename RecordType>
	static Record* FindByNameAndType(const StringId& Name)
	{
		Record* record = FindByName(Name);
		if (!record)
		{
			return nullptr;
		}
		return static_cast<RecordType>(record);
	}

	/*
	\brief		Registers the given record type.
	\tparam		DataType	Metadata class that defines the record.

	Will assert if a type with the same mangled name exists, as this is almost certainly a 
	failure to correctly unregister types.
	*/
	template <typename DataType>
	static void RegisterRecord()
	{
		DataType* NewType = LD_ARENA_NEW(ReflectionMemoryArena, DataType);

#ifndef LD_SHIPPING_BUILD
		for (Type* OldType : m_Records)
		{
			LD_ASSERT(OldType->m_UUID != NewType->m_UUID);
		}
#endif

		StringId typeId = StringId::Create(typeid(DataType).name());

		m_RecordsByFullName.Set(NewType->m_FullyQualifiedName, NewType);
		m_RecordsByName.Set(NewType->m_Name, NewType);
		m_RecordsByMetaTypeInfo.Set(typeId, NewType);
		m_Records.Push(NewType);
	}

	/*
	\brief		Unregisters a given record .
	\tparam		DataType	Metadata class that defines the record.

	Will assert if a type is not already registered, as this is almost certainly a
	failure to correctly unregister types.
	*/
	template <typename DataType>
	static void UnregisterRecord()
	{
		StringId typeId = StringId::Create(typeid(DataType).name());

		Record* instance;
		bool bTypeFound = m_RecordsByMetaTypeInfo.FindValue(typeId, instance);

		LD_ASSERT(bTypeFound);
		if (!bTypeFound)
		{
			return;
		}

		m_RecordsByFullName.RemoveValue(instance);
		m_RecordsByName.RemoveValue(instance);
		m_RecordsByMetaTypeInfo.RemoveValue(instance);
		m_Records.Remove(instance);

		LD_ARENA_DELETE(ReflectionMemoryArena, instance);
	}

	/* 
	\brief Called after all records have been registered, this call we build a hierarchical tree as
		   well as patching various interdependent references.
	*/
	static void FinalizeRegistration();

	// \brief Unregisters and destroys all type references, called on program shutdown.
	static void Teardown();

};

}; // namespace Ludo
