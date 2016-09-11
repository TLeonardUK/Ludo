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

#include "PCH.h"

#include "Core/Public/IO/Streams/StreamWrapper.h"

namespace Ludo {

StreamWrapper::StreamWrapper(IStream* BaseStream)
	: m_BaseStream(BaseStream)
{
}

StreamWrapper::~StreamWrapper()
{
	m_BaseStream = nullptr;
}

Error StreamWrapper::Write(void* Buffer, uint64 BufferLength)
{
	return m_BaseStream->Write(Buffer, BufferLength);
}

Error StreamWrapper::Read(void* Buffer, uint64 BufferLength)
{
	return m_BaseStream->Read(Buffer, BufferLength);
}

uint64 StreamWrapper::Position()
{
	return m_BaseStream->Position();
}

uint64 StreamWrapper::Length()
{
	return m_BaseStream->Length();
}

void StreamWrapper::Seek(uint64 Position)
{
	m_BaseStream->Seek(Position);
}

void StreamWrapper::Flush()
{
	m_BaseStream->Flush();
}

bool StreamWrapper::AtEnd()
{
	return m_BaseStream->AtEnd();
}

uint64 StreamWrapper::BytesLeft()
{
	return m_BaseStream->BytesLeft();
}

bool StreamWrapper::CanRead()
{
	return m_BaseStream->CanRead();
}

bool StreamWrapper::CanWrite()
{
	return m_BaseStream->CanWrite();
}

}; // namespace Ludo