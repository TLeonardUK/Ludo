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

#include "Core/Public/Reflection/Type.h"
#include "Core/Public/Reflection/Method.h"
#include "Core/Public/Reflection/Field.h"

namespace Ludo {

class Method;

/// \brief TODO
class Class : public Type
{
protected:
	friend class ReflectionFileScanner;

	Array<Method*> m_Methods;
	Array<Field*> m_Fields;

	Array<StringId> m_BaseClassesRaw;
	
	Array<Class*> m_BaseClasses;
	Array<Class*> m_DerivedClasses;
	bool m_bAbstract;

protected:
	void CollectParameterTypes(Array<Type*>& Result)
	{
		LD_UNUSED_PARAMETER(Result);
		// Termination function of templated version, intentially left empty.
	}

	template <typename Arg1, typename... Args>
	void CollectParameterTypes(Array<Type*>& Result, Arg1& x, Args&... args)
	{
		Result.Push(TypeOf(x));
		CollectParameterTypes(Result, args...);
	}

	void CollectParameters(Array<Type*>& Result)
	{
		LD_UNUSED_PARAMETER(Result);
		// Termination function of templated version, intentially left empty.
	}

	template <typename Arg1, typename... Args>
	void CollectParameters(Array<Type*>& Result, Arg1& x, Args&... args)
	{
		Result.Push(x);
		CollectParameters(Result, args...);
	}

	virtual void* NewInstance(IAllocator* Allocator);

public:
	Class();

	/// \brief TODO
	template <typename Arena, typename ResultType, typename... Args>
	ResultType* New(Args... args)
	{
		ArenaAllocator<Arena> Allocator;
		void* Instance = NewInstance(&Allocator);

		Array<Type*> ParameterTypes;
		CollectParameterTypes(ParameterTypes, args...);

		Method* Constructor = FindMethod(m_Name, TypeOfStatic(ResultType*), ParameterTypes);
		if (Constructor == nullptr)
		{
			String Signature = m_Name.ToString() + "(";
			for (int i = 0; i < ParameterTypes.Length(); i++)
			{
				Type* Type = ParameterTypes[i];
				if (i > 0)
				{
					Signature += ", ";
				}
				if (Type == nullptr)
				{
					Signature += "Unknown";
				}
				else
				{
					Signature += Type->GetFullName().ToString();
				}
			}
			Signature += ")";

			ConstantAssertMsgF("Failed to find appropriate constructor to create class instance by reflection, expected constructor with signature '%s'.", Signature.Data());
		}

		return Constructor->Invoke<ResultType*, Args...>(Instance, args...);
	}

	/// \brief TODO
	Method* FindMethod(const StringId& Name, Type* ReturnType, Array<Type*> ParameterTypes);

	/// \brief TODO
	template <typename... Args>
	Method* FindMethod(const StringId& Name, Type* ReturnType, Args... args)
	{
		Array<Type*> ParameterTypes;
		CollectParameters(ParameterTypes, args...);

		return FindMethod(Name, ReturnType, ParameterTypes);
	}

	/// \brief TODO
	Field* FindField(const StringId& Name, Type* DataType);

	/// \brief TODO
	bool IsAbstract();

	/// \brief TODO
	Array<Class*> GetBaseClasses();

	/// \brief TODO
	Array<Class*> GetDerivedClasses();

	/// \brief TODO
	Array<Field*>  GetFields();

	/// \brief TODO
	Array<Method*> GetMethods();

/*
	// Templates are not currently supported.
	bool IsTemplate();
	bool IsTemplateInstance();	
	Array<Type*>  GetTemplateTypes();
*/

};

}; // namespace Ludo
