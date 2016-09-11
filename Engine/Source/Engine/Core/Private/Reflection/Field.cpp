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

#include "Core/Public/Reflection/Field.h"

namespace Ludo {

Field::Field()
	: m_DataType(nullptr)
{
}

Type* Field::GetDataType()
{
	return m_DataType;
}

bool Field::IsStatic()
{
	return m_bStatic;
}

bool Field::IsConst()
{
	return m_bConst;
}

void Field::SetInternal(void* Instance, void* Data)
{
	UNUSED_PARAMETER(Instance);
	UNUSED_PARAMETER(Data);
	PureVirtual();
}

void Field::GetInternal(void* Instance, void* Data)
{
	UNUSED_PARAMETER(Instance);
	UNUSED_PARAMETER(Data);
	PureVirtual();
}

}; // namespace Ludo