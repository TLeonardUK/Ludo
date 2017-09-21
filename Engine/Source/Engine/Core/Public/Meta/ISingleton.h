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

/** \brief When a class implements this interface it will only ever have a single instance.
 *         The instance can be retrieved from the Instance member.
 *
 *  \tparam Type of class to host a singleton instance of. This should be the same as the 
 *          class that is implementing this interface.
 *
 *  Instance is implemented as a static value-type. Time of construction is undefined, but will
 *  always be constructed when you access it.
 */
template <typename BaseType>
class ISingleton
{
public:

    /// \brief Primary instance of this singleton.
    static BaseType Instance;

};

};
