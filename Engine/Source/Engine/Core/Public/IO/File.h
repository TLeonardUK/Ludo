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
 
/** \brief Mode to open file in. Determines what operations
 *         can be performed on the file.
 */
enum class FileFlags
{
	Read		= 1,
	Write		= 2,
	ReadWrite	= Read | Write,
};

/** \brief Represents a file on the platforms file system. Provides a stream
 *         interface to read and write to it.
 */
class File : 
	public IStream, 
	public IPimpl,
	public INotCopyable
{
public:

	/// \brief Constructs a new file instance.
	File();

    ~File();
    
	/** \brief Opens the file on the platforms file system with the given mode/
     *
     *  \param  Path  Path to the file to open.
     *  \param  Flags Flags determining what mode the file should be opened in.
     *
     *  \returns Error value indicating success or failure.
     */
	Error Open(const Path& Path, FileFlags Flags);

	/// \brief Closes access to the file. This is implicitly called during destruction.
	void Close();

	/** \brief Returns true if this file is currently open and accessible.
     *
     *  \returns True if file is open and accessible.
     */
	bool IsOpen();

    // IStream 
	virtual Error Write(void* Buffer, uint64 BufferLength) override;
	virtual Error Read(void* Buffer, uint64 BufferLength) override;
	virtual uint64 Position() override;
	virtual uint64 Length() override;
	virtual void Seek(uint64 Position) override;
	virtual bool AtEnd() override;
	virtual void Flush() override;
	virtual bool CanRead() override;
	virtual bool CanWrite() override;
    // End IStream 

};

};