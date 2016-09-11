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

#include "Core/Public/Types/String.h"

namespace Ludo {

/// \brief TODO
enum class VariantType
{
	Int,
	Float,
	Double,
	String
};

/// \brief TODO
struct Variant
{
public:
	VariantType Type;

	union
	{
		int			IntValue;
		float		FloatValue;
		double		DoubleValue;
	};

	String StringValue;

	Variant(int Value)
		: Type(VariantType::Int)
		, IntValue(Value)
	{
	}

	Variant(float Value)
		: Type(VariantType::Float)
		, FloatValue(Value)
	{
	}

	Variant(double Value)
		: Type(VariantType::Double)
		, DoubleValue(Value)
	{
	}

	Variant(String Value)
		: Type(VariantType::String)
		, StringValue(Value)
	{
	}

};

};