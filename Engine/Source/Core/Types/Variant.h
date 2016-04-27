// Copyright 2016 TwinDrills
#pragma once

#include "Core/Types/String.h"

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