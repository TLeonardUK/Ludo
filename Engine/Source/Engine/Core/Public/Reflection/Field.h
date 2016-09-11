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
class Field : public Type
{
protected:
	friend class ReflectionFileScanner;

	StringId m_ClassNameRaw;

	Type*    m_DataType;
	StringId m_DataTypeRaw;
	StringId m_DataTypeRawRefRemoved;

	bool m_bStatic;
	bool m_bConst;

protected:
	virtual void SetInternal(void* Instance, void* Data);
	virtual void GetInternal(void* Instance, void* Data);

public:
	Field();

	/// \brief TODO
	Type* GetDataType();

	/// \brief TODO
	bool IsStatic();

	/// \brief TODO
	bool IsConst();

	/// \brief TODO
	template <typename DataType>
	DataType GetValue(void* Instance)
	{
		DataType Value;
		GetInternal(Instance, reinterpret_cast<void*>(&Value));
		return Value;
	}

	/// \brief TODO
	template <typename DataType>
	void SetValue(void* Instance, DataType Type)
	{
		SetInternal(Instance, reinterpret_cast<void*>(&Type));
	}

};

}; // namespace Ludo

