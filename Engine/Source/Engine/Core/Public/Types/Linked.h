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
template 
<
	typename ElementType
>
class Linked
{
public:

	/// \brief TODO
	Linked()
		: m_Next(nullptr)
	{
	}

	/// \brief TODO
	void Link(Linked<ElementType>* & other)
	{
		this->m_Next = other;
		other = this;
	}

	/// \brief TODO
	void Unlink(Linked<ElementType>*& other)
	{
		other = this->m_Next;
	}

	/// \brief TODO
	Linked<ElementType>* Next() const
	{
		return m_Next;
	}

	/// \brief TODO
	ElementType& operator*()
	{
		return static_cast<ElementType*>(this);
	}

	/// \brief TODO
	ElementType* operator->()
	{
		return static_cast<ElementType*>(this);
	}

protected:
	Linked<ElementType>* m_Next;

};

};