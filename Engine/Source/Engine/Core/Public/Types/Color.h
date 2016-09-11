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

#include "Core/Public/Types/Array.h"

namespace Ludo {

/// \brief TODO
class Color
{
public:
	static Color Red;
	static Color Green;
	static Color Blue;
	static Color Yellow;
	static Color White;
	static Color Black;

	float R, G, B, A;

	/// \brief TODO
	Color()
		: R(1.0f)
		, G(1.0f)
		, B(1.0f)
		, A(1.0f)
	{
	}

	/// \brief TODO
	Color(int InR, int InG, int InB, int InA)
		: R(InR / 255.0f)
		, G(InG / 255.0f)
		, B(InB / 255.0f)
		, A(InA / 255.0f)
	{
	}

	/// \brief TODO
	Color(int InR, int InG, int InB)
		: R(InR / 255.0f)
		, G(InG / 255.0f)
		, B(InB / 255.0f)
		, A(1.0f)
	{
	}
	
	/// \brief TODO
	Color(float InR, float InG, float InB, float InA)
		: R(InR)
		, G(InG)
		, B(InB)
		, A(InA)
	{
	}

	/// \brief TODO
	Color(float InR, float InG, float InB)
		: R(InR)
		, G(InG)
		, B(InB)
	{
	}

	/// \brief TODO
	Color(unsigned int Argb)
		: A(((Argb >> 24) & 0xFF) / 255.0f)
		, R(((Argb >> 16) & 0xFF) / 255.0f)
		, G(((Argb >> 8) & 0xFF) / 255.0f)
		, B(((Argb) & 0xFF) / 255.0f)
	{
	}

	/// \brief TODO
	Color(const Color& Type)
		: R(Type.R)
		, G(Type.G)
		, B(Type.B)
		, A(Type.A)
	{
	}

	/// \brief TODO
	unsigned char ToA8()
	{
		return (unsigned char)(A * 255.0f);
	}

	/// \brief TODO
	unsigned char ToR8()
	{
		return (unsigned char)(R * 255.0f);
	}

	/// \brief TODO
	unsigned char ToG8()
	{
		return (unsigned char)(G * 255.0f);
	}

	/// \brief TODO
	unsigned char ToB8()
	{
		return (unsigned char)(B * 255.0f);
	}

	/// \brief TODO
	unsigned int ToArgb()
	{
		return ((ToA8() << 24) | (ToR8() << 16) | (ToG8() << 8) | (ToB8()));
	}

	/// \brief TODO
	float DistanceTo(const Color& Other)
	{
	//	return sqrtf(Math::Power(Other.A - A, 2.0f) + Math::Power(Other.R - R, 2.0f) + Math::Power(Other.G - G, 2.0f) + Math::Power(Other.B - B, 2.0f));
		return sqrtf(fabs(Other.A - A) + fabs(Other.R - R) + fabs(Other.G - G) + fabs(Other.B - B));
	}

	/// \brief TODO
	int FindClosestPaletteIndex(const Array<Color>& Pallete)
	{
		float ClosestDistance = 0;
		int ClosestEntry = 0;

		for (int i = 0; i < Pallete.Length(); i++)
		{
			const Color& Entry = Pallete[i];
			float Distance = DistanceTo(Entry);
			if (i == 0 || Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestEntry = i;
			}
		}
		
		return ClosestEntry;
	}

	static Color FromAbgr(unsigned int Abgr)
	{
		Color c;
		c.A = (((Abgr >> 24) & 0xFF) / 255.0f);
		c.B = (((Abgr >> 16) & 0xFF) / 255.0f);
		c.G = (((Abgr >> 8) & 0xFF) / 255.0f);
		c.R = (((Abgr)& 0xFF) / 255.0f);
		return c;
	}

};

};