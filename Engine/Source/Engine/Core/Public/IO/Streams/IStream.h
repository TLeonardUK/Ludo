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

#include "Core/Public/Build/Common.h"
#include "Core/Public/Types/Error.h"

namespace Ludo {

/// \brief TODO
class IStream 
{
public:

	/// \brief TODO
	virtual Error Write(void* Buffer, uint64 BufferLength) = 0;

	/// \brief TODO
	virtual Error Read(void* Buffer, uint64 BufferLength) = 0;

	/// \brief TODO
	virtual uint64 Position() = 0;

	/// \brief TODO
	virtual uint64 Length() = 0;

	/// \brief TODO
	virtual void Seek(uint64 Position) = 0;

	/// \brief TODO
	virtual void Flush();

	/// \brief TODO
	virtual bool AtEnd();

	/// \brief TODO
	virtual uint64 BytesLeft();

	/// \brief TODO
	virtual bool CanRead() = 0;

	/// \brief TODO
	virtual bool CanWrite() = 0;

};

};