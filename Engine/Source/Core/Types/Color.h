// Copyright 2016 TwinDrills
#pragma once

#include "Core/Math/Math.h"

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
		return (unsigned char)Math::Clamp(A * 255.0f, 0.0f, 255.0f);
	}

	/// \brief TODO
	unsigned char ToR8()
	{
		return (unsigned char)Math::Clamp(R * 255.0f, 0.0f, 255.0f);
	}

	/// \brief TODO
	unsigned char ToG8()
	{
		return (unsigned char)Math::Clamp(G * 255.0f, 0.0f, 255.0f);
	}

	/// \brief TODO
	unsigned char ToB8()
	{
		return (unsigned char)Math::Clamp(B * 255.0f, 0.0f, 255.0f);
	}

	/// \brief TODO
	unsigned int ToArgb()
	{
		return ((ToA8() << 24) | (ToR8() << 16) | (ToG8() << 8) | (ToB8()));
	}

};

};