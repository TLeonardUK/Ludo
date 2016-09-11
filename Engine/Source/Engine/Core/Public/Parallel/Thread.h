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
#include "Core/Public/Types/String.h"

namespace Ludo {

/// \brief TODO
class Thread :
	public IPimpl,
	public INotCopyable
{
protected:
	friend class ThreadImpl;

	/// \brief TODO
	bool IsEqual(const Thread* other) const;

public:

	/// \brief TODO
	typedef std::function<void (Thread* Self)> EntryPoint;

	/// \brief TODO
	Thread();

	/// \brief TODO
	Thread(bool bCreateOwnThread);

	/// \brief TODO
	~Thread();

	/// \brief TODO
	void SetName(const String& Name);

	/// \brief TODO
	void Start(EntryPoint Point);

	/// \brief TODO
	void Abort();

	/// \brief TODO
	void Join();

	/// \brief TODO
	bool IsRunning();

	/// \brief TODO
	bool IsAborted();

	/// \brief TODO
	bool operator==(const Thread& other) const;
	bool operator!=(const Thread& other) const;

	/// \brief TODO
	static Thread* Self();

	/// \brief TODO
	static Thread* Main();

	/// \brief TODO
	static void EnterMainThread();

	/// \brief TODO
	static void ExitMainThread();

};

};