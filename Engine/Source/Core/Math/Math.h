// Copyright 2016 TwinDrills
#pragma once

#include "Core/Math/Math.h"

#include <math.h>
#include <stdarg.h>

namespace Ludo {

class Math
{
public:

	/// \brief: TODO
	static int GetNextPowerOfTwo(int Value)
	{
		double Result = Power(2.0, Ceiling(LogX(Value, 2.0)));
		return static_cast<int>(Result);
	}

	/// \brief: TODO
	static double Power(double X, double Y)
	{
		return pow(X, Y);
	}

	/// \brief: TODO
	static double Ceiling(const double Value)
	{
		return ceil(Value);
	}

	/// \brief: TODO
	static double Floor(const double Value)
	{
		return floor(Value);
	}

	/// \brief: TODO
	static double LogX(const double Value, const double Base)
	{
		double Result = (log(Value) / log(Base));
		return Result;
	}

	/// \brief: TODO
	template <typename T>
	static T Clamp(T V, T MinV, T MaxV)
	{
		if (V < MinV)
		{
			V = MinV;
		}
		if (V > MaxV)
		{
			V = MaxV;
		}
		return V;
	}

	/// \brief: TODO
	template <typename T>
	static T Min(T X, T Y)
	{
		return (X < Y ) ? X : Y;
	}

	/// \brief: TODO
	template <typename T>
	static T Max(T X, T Y)
	{
		return (X > Y) ? X : Y;
	}

};

};