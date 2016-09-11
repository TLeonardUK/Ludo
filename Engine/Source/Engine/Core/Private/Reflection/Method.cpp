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

#include "Core/Public/Reflection/Method.h"

namespace Ludo {

Method::Method()
	: m_ReturnType(nullptr)
{
}

Array<Parameter> Method::GetParameters()
{
	return m_Parameters;
}

Type* Method::GetReturnType()
{
	return m_ReturnType;
}

void Method::InvokeInternal(void* Instance, void* Result, Array<void*>& Parameters)
{
	UNUSED_PARAMETER(Instance);
	UNUSED_PARAMETER(Result);
	UNUSED_PARAMETER(Parameters);
	PureVirtual();
}

bool Method::IsConstructor()
{
	return m_bConstructor;
}

bool Method::IsStatic()
{
	return m_bStatic;
}

bool Method::IsConst()
{
	return m_bConst;
}

bool Method::IsVirtual()
{
	return m_bVirtual;
}

bool Method::IsPureVirtual()
{
	return m_bPureVirtual;
}

}; // namespace Ludo