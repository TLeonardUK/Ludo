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

/** \brief Base interface for input-output stream implementation (such as files, sockets, etc).
 *
 * Always use asyncronous streams when possible.
 */
class IStream 
{
public:

	/** \brief  Syncronously writes a buffer of bytes to a stream. The position
     *          in the stream is not updated.
     *
     *  \param  Buffer Buffer of bytes to write.
     *  \param  Length Number of bytes to write.
     *  \param  Offset Offset to write to..
     *
     *  \return Error value determining success.
     */
	//virtual Future<AsyncFileOperation> AsyncWrite(void* Buffer, uint64 Length, uint64 Offset) = 0;

    /** \brief  Asyncronously reads a buffer of bytes to a stream. The position
     *          in the stream is not updated.
     *
     *  \param  Buffer Buffer of bytes to read.
     *  \param  Length Number of bytes to read.
     *  \param  Offset Offset to read from.
     *
     *  \return Error value determining success.
     */
	//virtual Future<AsyncFileOperation> AsyncRead(void* Buffer, uint64 Length, uint64 Offset) = 0;

	/** \brief  Syncronously writes a buffer of bytes to a stream and increments
     *          the position in the stream by the buffer length.
     *
     *  \param  Buffer Buffer of bytes to write.
     *  \param  Length Number of bytes to write.
     *
     *  \return Error value determining success.
     */
	virtual Error Write(void* Buffer, uint64 BufferLength) = 0;

    /** \brief  Syncronously reads a buffer of bytes to a stream and increments
     *           the position in the stream by the buffer length.
     *
     *  \param  Buffer Buffer of bytes to read.
     *  \param  Length Number of bytes to read.
     *
     *  \return Error value determining success.
     */
	virtual Error Read(void* Buffer, uint64 Length) = 0;

    /** \brief  Gets the position offset in bytes in stream that we 
    *           are currently reading or writing to.
     *
     *  \return Position in stream in bytes.
     */
	virtual uint64 Position() = 0;
    
    /** \brief  Gets the total length of stream in bytes.
     *
     *  \return Total length of stream in bytes.
     */
	virtual uint64 Length() = 0;
    
    /** \brief  Moves the position we are currently reading and writing
     *          to, to the given position offset.
     *
     *  \param  Position Offset to move to in bytes.
     */
	virtual void Seek(uint64 Position) = 0;
    
    /// \brief  Flushes any pending writes in any internal buffers. This is implicitly
    ///         called when you destroy a stream.
	virtual void Flush();
    
    /** \brief  Gets if we have read to the end of a stream.
     *
     *  \return If we are at the end of a stream.
     */
	virtual bool AtEnd();
    
    /** \brief  Gets the number of bytes left to read.
     *
     *  \return Number of bytes left to read.
     */
	virtual uint64 BytesLeft();
    
    /** \brief  Gets if this stream can be read from.
     *
     *  \return If this stream can be read from.
     */
	virtual bool CanRead() = 0;
    
    /** \brief  Gets if this stream can be written to.
     *
     *  \return If this stream can be written to.
     */
	virtual bool CanWrite() = 0;

};

};