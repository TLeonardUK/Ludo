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

#include "Core/Public/IO/Streams/IStream.h"
#include "Core/Public/Meta/Traits/INotCopyable.h"
#include "Core/Public/Meta/IPimpl.h"
#include "Core/Public/Types/Error.h"
#include "Core/Public/IO/Path.h"
#include "Core/Public/Environment/Environment.h"

namespace Ludo {

/// \brief TODO
enum class FileFlags
{
	Read		= 1,
	Write		= 2,
	ReadWrite	= Read | Write,
};

/// \brief TODO
class File : 
	public IStream, 
	public IPimpl,
	public INotCopyable
{
public:

	/// \brief TODO
	File();

	/// \brief TODO
	~File();

	/// \brief TODO
	Error Open(const Path& path, FileFlags flags);

	/// \brief TODO
	void Close();

	/// \brief TODO
	bool IsOpen();

	/// \brief TODO
	virtual Error Write(void* Buffer, uint64 BufferLength) override;

	/// \brief TODO
	virtual Error Read(void* Buffer, uint64 BufferLength) override;

	/// \brief TODO
	virtual uint64 Position() override;

	/// \brief TODO
	virtual uint64 Length() override;

	/// \brief TODO
	virtual void Seek(uint64 Position) override;

	/// \brief TODO
	virtual bool AtEnd() override;

	/// \brief TODO
	virtual void Flush() override;

	/// \brief TODO
	virtual bool CanRead() override;

	/// \brief TODO
	virtual bool CanWrite() override;

};

/// \brief TODO
struct FileMutex
{
private:
	File m_File;

public:
	FileMutex(const Path& PathToFile)
	{
		while (true)
		{
			m_File.Open(PathToFile, FileFlags::Write);
			if (m_File.IsOpen())
			{
				break;
			}
			Environment::Delay(TimeSpan(1000LLU));
		}
	}

	~FileMutex()
	{
		m_File.Close();
	}

};

};