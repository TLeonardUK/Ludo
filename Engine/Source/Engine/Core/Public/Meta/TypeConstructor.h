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
		LD_UNUSED_PARAMETER(Ref); // Not sure why VS requires this...
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