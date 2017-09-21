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

/** \brief Helper class for constructing and destructing types. 
 *
 *  \tparam ElementType Type of element to construct.
 *
 *  For value types this results in the constructor/destructor being called, 
 *  for pointer types this results in the value being nulled out. This is primarily 
 *  useful for constructing arrays of unknown type or other templated data-structures.
 */
template <typename ElementType>
struct TypeConstructor
{
    /** \brief Calls default constructor of the given value type.
     *
     *  \param Ref Reference to value type to construct.
     */
    static void Construct(ElementType& Ref)
    {
        new (&Ref) ElementType();
    }

    /** \brief Calls default destructor of the given value type.
     *
     *  \param Ref Reference to value type to destruct.
     */
    static void Destruct(ElementType& Ref)
    {
        LD_UNUSED_PARAMETER(Ref); // Not sure why VS requires this...
        Ref.~ElementType();
    }
};

/** \brief Type constructor specialization for pointer types. Read comments for the base TypeConstructor
 *         for further details.
 *
 *  \tparam ElementType Type of element to construct.
 */
template <typename ElementType>
struct TypeConstructor<ElementType*>
{
    /** \brief Nulls out the memory location holding the given pointer.
     *
     *  \param Ref Reference to pointer to null out.
     */
    static void Construct(ElementType*& Ref)
    {
        Ref = nullptr;
    }
    
    /** \brief Nulls out the memory location holding the given pointer.
     *
     *  \param Ref Reference to pointer to null out.
     */
    static void Destruct(ElementType*& Ref)
    {
        Ref = nullptr;
    }
};

};
