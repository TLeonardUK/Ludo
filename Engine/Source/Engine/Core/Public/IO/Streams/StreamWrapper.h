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
#include "Core/Public/IO/Streams/IStream.h"

namespace Ludo {

/** \brief This is the base class for all stream wrappers. Stream wrappers take a base stream
 *         and provides the same stream interface, but typically perform or provide some additional 
 *         intermediate functionality.
 */
class StreamWrapper 
    : public IStream
{
private:
	IStream* m_BaseStream;

public:

	StreamWrapper(IStream* BaseStream);
	virtual ~StreamWrapper();

public:

    // IStream 
	virtual Error Write(void* Buffer, uint64 BufferLength) override;
	virtual Error Read(void* Buffer, uint64 BufferLength) override;
	virtual uint64 Position() override;
	virtual uint64 Length() override;
	virtual void Seek(uint64 Position) override;
	virtual void Flush() override;
	virtual bool AtEnd() override;
	virtual uint64 BytesLeft() override;
	virtual bool CanRead() override;
	virtual bool CanWrite() override;
    // End IStream 

};

};