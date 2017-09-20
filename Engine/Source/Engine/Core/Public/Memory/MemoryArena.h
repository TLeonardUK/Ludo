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
class IMemoryArena
{
public:
    
    /** \brief TODO
     * 
     *  \returns
     */
	virtual const char* GetName() const = 0;
    
    /** \brief TODO
     * 
     *  \returns
     */
	virtual const char* GetDescription() const = 0;
    
    /** \brief TODO
     * 
     *  \param bResizable
     *  \param DefaultSize
     */
	virtual void GetDefaultState(bool& bResizable, int& DefaultSize) const = 0;
    
    /** \brief TODO
     * 
     *  \param Count
     *
     *  \returns
     */
	virtual void* Allocate(const int Count) = 0;
    
    /** \brief TODO
     * 
     *  \param Ptr
     *  \param Count
     *
     *  \returns
     */
	virtual void* Reallocate(void* Ptr, const int Count) = 0;
    
    /** \brief TODO
     * 
     *  \param Ptr
     */
	virtual void Free(void* Ptr) = 0;

};

};