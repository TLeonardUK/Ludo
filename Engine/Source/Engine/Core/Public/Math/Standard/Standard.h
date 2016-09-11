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

#include <math.h>
#include <stdarg.h>

namespace Ludo {
namespace Math {

/// \brief: TODO
template <typename T>
T Power(T X, T Y)
{
	return (T)pow(X, Y);
}

/// \brief: TODO
template <typename T>
T Ceiling(const T Value)
{
	return (T)ceil(Value);
}

/// \brief: TODO
template <typename T>
T Floor(const T Value)
{
	return (T)floor(Value);
}

/// \brief: TODO
template <typename T>
T LogX(const T Value, const T Base)
{
	T Result = (T)(log(Value) / log(Base));
	return Result;
}

/// \brief: TODO
template <typename T>
T Clamp(T V, T MinV, T MaxV)
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
T Min(T X, T Y)
{
	return (X < Y) ? X : Y;
}

/// \brief: TODO
template <typename T>
T Max(T X, T Y)
{
	return (X > Y) ? X : Y;
}

/// \brief: TODO
template <typename T>
int GetNextPowerOfTwo(T Value)
{
	float Result = Power(2.0f, Ceiling(LogX((float)Value, 2.0f)));
	return static_cast<T>(Result);
}

}; // namespace Math
}; // namespace Ludo