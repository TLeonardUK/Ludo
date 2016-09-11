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

/// \brief TODO
class Process :
	public IStream,
	public IPimpl,
	public INotCopyable
{
public:

	/// \brief TODO
	Process();

	/// \brief TODO
	~Process();

	/// \brief TODO
	Error Open(const Path& Command, const Path& WorkingDirectory, const Array<String>& Arguments, bool bRedirectStdInOut = false);

	/// \brief TODO
	void Detach();

	/// \brief TODO
	void Terminate();

	/// \brief TODO
	bool IsRunning();

	/// \brief TODO
	bool IsAttached();

	/// \brief TODO
	bool Wait(TimeSpan Timeout = TimeSpan::Infinite);

	/// \brief TODO
	int GetExitCode();

	/// \brief TODO
	Error Write(void* Buffer, uint64 BufferLength) override;

	/// \brief TODO
	Error Read(void* Buffer, uint64 BufferLength) override;

	/// \brief TODO
	uint64 Position() override;

	/// \brief TODO
	uint64 Length() override;

	/// \brief TODO
	void Seek(uint64 Position) override;

	/// \brief TODO
	bool AtEnd() override;

	/// \brief TODO
	void Flush() override;

	/// \brief TODO
	uint64 BytesLeft() override;

	/// \brief TODO
	bool CanRead() override;

	/// \brief TODO
	bool CanWrite() override;

};

};