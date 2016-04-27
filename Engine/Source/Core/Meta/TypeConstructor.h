// Copyright 2016 TwinDrills
#pragma once

namespace Ludo {

/// \brief TODO
template <typename ElementType>
struct TypeConstructor
{
	static void Construct(ElementType& Ref)
	{
		new (&Ref) ElementType();
	}
	static void Destruct(ElementType& Ref)
	{
		UNUSED_PARAMETER(Ref); // Not sure why VS requires this...
		Ref.~ElementType();
	}
};

/// \brief TODO
template <typename ElementType>
struct TypeConstructor<ElementType*>
{
	static void Construct(ElementType*& Ref)
	{
		Ref = nullptr;
	}
	static void Destruct(ElementType*& Ref)
	{
		Ref = nullptr;
	}
};

};