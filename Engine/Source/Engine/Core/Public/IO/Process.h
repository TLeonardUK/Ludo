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

#include "Core/Public/Meta/IPimpl.h"
#include "Core/Public/Meta/Traits/INotCopyable.h"
#include "Core/Public/Types/Error.h"
#include "Core/Public/IO/Streams/IStream.h"
#include "Core/Public/IO/Path.h"

namespace Ludo {
    
/** \brief Represents a executable process running, or to be run, on the host platform. 
 *         Provides a stream interface to read and write from the running process.
 */
class Process :
	public IStream,
	public IPimpl,
	public INotCopyable
{
public:

	Process();
	~Process();
    
	/** \brief Attempts to start a process of the given executable with the given properties.
     * 
     *  \param Command              Path to executable, or name of command, to execute. 
     *                              Depending on the platform if this is relative, the host platform may look through 
     *                              common directories (typically in the PATH envvar) to find the requested executable.
     *  \param WorkingDirectory     Directory process should start in.
     *  \param Arguments            Command line arguments to pass to process. It is not required to pass in the command
     *                              in this argument list (like argv).
     *  \param bRedirectStdInOut    If set the standard pipes will be captured so input and output can be read and writte
     *                              from the process using this classes stream interface.
     *
     *  \returns Error value determining success.
     */
	Error Open(const Path& Command, const Path& WorkingDirectory, const Array<String>& Arguments, bool bRedirectStdInOut = false);
    
	/** \brief Detaches from the process and frees any associated resources. This is implicitly called on destruction.
     *
     *  Once not attached to the process, no other functionality is valid as we are no longer
     *  able to monitor or influence it.
     */
	void Detach();

	/** \brief Sends a single to the process to tell it to terminate itself.
     *
     * This is not guaranteed to terminate the process, as on many platforms the process 
     * can capture and ignore any requests to terminate it.
     */
	void Terminate();
    
	/** \brief Returns true if the process is currently running.
     *
     *  \returns True if process is currently running.
     */
	bool IsRunning();
    
	/** \brief Returns true if we are currently attached to the process and able to monitor its state.
     *
     *  \returns True if we are currently attached to the process.
     *
     *  If not attached to the process, no other functionality is valid as we are no longer
     *  able to monitor or influence it.
     */
	bool IsAttached();
    
	/** \brief Pauses execution of current thread until the process exits.
     *
     *  \param Timeout How long to wait for the process to exit before
     *                 resuming. By default this will wait indefinitely.
     *
     *  \returns True if we resumed due to process exit, false if we resumed
     *           due to other reason (timeout, spurious wakeup, etc).
     */
	bool Wait(TimeSpan Timeout = TimeSpan::Infinite);
    
	/** \brief Returns the exit code reported to the operating system when this process shutdown.
     *
     *  \returns Processes reported exit code.
     *
     *  It is not valid to call this function while the process is still running.
     */
	int GetExitCode();

    // IStream 
    virtual Error Write(void* Buffer, uint64 BufferLength) override;
    virtual Error Read(void* Buffer, uint64 BufferLength) override;
    virtual uint64 Position() override;
    virtual uint64 Length() override;
    virtual void Seek(uint64 Position) override;
    virtual bool AtEnd() override;
    virtual void Flush() override;
    virtual uint64 BytesLeft() override;
	virtual bool CanRead() override;
    virtual bool CanWrite() override;
    // End IStream 

};

};