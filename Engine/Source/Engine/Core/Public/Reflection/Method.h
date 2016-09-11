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

namespace Ludo {

/// \brief TODO
class Parameter
{
public:
	Parameter()
		: DataType(nullptr)
	{
	}

	Parameter(Type* dataType, StringId name)
		: DataType(dataType)
		, Name(name)
	{
	}

	Type*    DataType;
	StringId DataTypeRaw;
	StringId DataTypeRawRefRemoved;
	StringId Name;
};

/// \brief TODO
class Method : public Type
{
protected:
	friend class ReflectionFileScanner;

	Array<Parameter> m_Parameters;
	Type* m_ReturnType;

	StringId m_ClassNameRaw;
	StringId m_ReturnTypeRaw;
	StringId m_ReturnTypeRawRefRemoved;

	bool m_bConstructor;
	bool m_bStatic;
	bool m_bConst;
	bool m_bVirtual;
	bool m_bPureVirtual;

protected:
	virtual void InvokeInternal(void* Instance, void* Result, Array<void*>& Parameters);

	/// \brief TODO
	template <typename ResultType, typename... Args>
	struct MethodInvoker
	{
		static ResultType Invoke(Method* Method, Array<void*>& ParametersPtrs, void* Instance, Args... args)
		{
			ResultType Result;
			Method->InvokeInternal(Instance, reinterpret_cast<void*>(&Result), ParametersPtrs);
			return Result;
		}
	};

	/// \brief TODO
	template <typename... Args>
	struct MethodInvoker<void, Args...>
	{
		static void Invoke(Method* Method, Array<void*>& ParametersPtrs, void* Instance, Args... args)
		{
			Method->InvokeInternal(Instance, nullptr, ParametersPtrs);
		}
	};

	void CollectParameterPointers(Array<void*>& Result, int ParamIndex)
	{
		UNUSED_PARAMETER(Result);
		UNUSED_PARAMETER(ParamIndex);

		// Termination function of templated version, intentially left empty.
	}

	template <typename Arg1, typename... Args>
	void CollectParameterPointers(Array<void*>& Result, int ParamIndex, Arg1& x, Args&... args)
	{
		Parameter* Param = &m_Parameters[ParamIndex];

#ifndef LD_SHIPPING_BUILD
		Type* PassedType = TypeOfStatic(Arg1);
		if (Param->DataType != PassedType)
		{
			ConstantAssertMsgF("Argument %i passed to %s by reflection is of incorrect type '%s', expected '%s'.", 
				ParamIndex, 
				m_FullName.ToString().Data(),
				PassedType == nullptr ? "Unknown" : PassedType->GetFullName().ToString().Data(),
				Param->DataType == nullptr ? "Unknown" : Param->DataType->GetFullName().ToString().Data()
			);
		}
#endif

		Result.Push(reinterpret_cast<void*>(&x));
		CollectParameterPointers(Result, ParamIndex + 1, args...);
	}

public:
	Method();

	/// \brief TODO
	template <typename ResultType, typename... Args>
	ResultType Invoke(void* Instance, Args... args)
	{
#ifndef LD_SHIPPING_BUILD
		Type* PassedType = TypeOfStatic(ResultType);
		if (m_ReturnType != PassedType)
		{
			ConstantAssertMsgF("Return type passed to %s by reflection is of incorrect type '%s', expected '%s'.",
				m_FullName.ToString().Data(),
				PassedType == nullptr ? "Unknown" : PassedType->GetFullName().ToString().Data(),
				m_ReturnType == nullptr ? "Unknown" : m_ReturnType->GetFullName().ToString().Data()
			);
		}
#endif

		Array<void*> ParameterPtrs;
		CollectParameterPointers(ParameterPtrs, 0, args...);

#ifndef LD_SHIPPING_BUILD
		if (ParameterPtrs.Length() != m_Parameters.Length())
		{
			ConstantAssertMsgF("Incorrect number of parameters passed to %s by reflection. Got %i, expected %i.",
				m_FullName.ToString().Data(),
				ParameterPtrs.Length(),
				m_Parameters.Length()
			);
		}
#endif

		return MethodInvoker<ResultType, Args...>::Invoke(this, ParameterPtrs, Instance, args...);
	}

	/// \brief TODO
	Array<Parameter> GetParameters();

	/// \brief TODO
	Type* GetReturnType();

	/// \brief TODO
	bool IsConstructor();

	/// \brief TODO
	bool IsStatic();

	/// \brief TODO
	bool IsConst();

	/// \brief TODO
	bool IsVirtual();

	/// \brief TODO
	bool IsPureVirtual();

};

}; // namespace Ludo

