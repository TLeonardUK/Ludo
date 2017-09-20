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

/** \brief Gets value raised to the power Y.
 *
 *  \tparam T Datatype to operate on.
 *
 *  \param  X Value to raise.
 *  \param  Y Power to raise by.
 *
 *  \returns Value raised to the power Y.
 */
template <typename T>
T Power(T X, T Y)
{
	return (T)pow(X, Y);
}

/** \brief Gets value rounded up to the nearest integer.
 *
 *  \tparam T Datatype to operate on.
 *
 *  \param  X Value to round up.
 *
 *  \returns Value rounded up to the nearest integer.
 */
template <typename T>
T Ceiling(const T Value)
{
	return (T)ceil(Value);
}

/** \brief Gets value rounded down to the nearest integer.
 *
 *  \tparam T Datatype to operate on.
 *
 *  \param  X Value to round down.
 *
 *  \returns Value rounded down to the nearest integer.
 */
template <typename T>
T Floor(const T Value)
{
	return (T)floor(Value);
}

/** \brief Returns the logarithim of value in the given base. 
 *
 *  \tparam T Datatype to operate on.
 *
 *  \param  Value Value to get logarithim of.
 *  \param  Base  Base to get logarithim in.
 *
 *  \returns Returns the logarithim of value in the given base. 
 */
template <typename T>
T LogX(const T Value, const T Base)
{
	T Result = (T)(log(Value) / log(Base));
	return Result;
}

/** \brief Clamps a value between a given upper and lower bound.
 *
 *  \tparam T Datatype to operate on.
 *
 *  \param  Value Value to clamp.
 *  \param  MinV  Minimum bounds of value.
 *  \param  MaxV  Maximum bounds of value.
 *
 *  \returns Value clamped between upper and lower bound.
 */
template <typename T>
T Clamp(T Value, T MinV, T MaxV)
{
	if (Value < MinV)
	{
        Value = MinV;
	}
	if (Value > MaxV)
	{
        Value = MaxV;
	}
	return Value;
}

/** \brief Returns the lower of two values.
 *
 *  \tparam T Datatype to operate on.
 *
 *  \param  X First value.
 *  \param  Y Second value.
 *
 *  \returns Lower of two values.
 */
template <typename T>
T Min(T X, T Y)
{
	return (X < Y) ? X : Y;
}

/** \brief Returns the higher of two values.
 *
 *  \tparam T Datatype to operate on.
 *
 *  \param  X First value.
 *  \param  Y Second value.
 *
 *  \returns Higher of two values.
 */
template <typename T>
T Max(T X, T Y)
{
	return (X > Y) ? X : Y;
}

/** \brief Returns the next power of two above the given value.
 *
 *  \tparam T Datatype to operate on.
 *
 *  \param  Value Value to get next power of two from.
 *
 *  \returns Next power above the given value.
 */
template <typename T>
int GetNextPowerOfTwo(T Value)
{
	float Result = Power(2.0f, Ceiling(LogX((float)Value, 2.0f)));
	return static_cast<T>(Result);
}

}; // namespace Math
}; // namespace Ludo