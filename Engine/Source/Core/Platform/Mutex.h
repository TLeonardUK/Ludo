// Copyright 2016 TwinDrills
#pragma once

#include "Core/Types/String.h"

namespace Ludo {

/// \brief TODO
class Mutex
{
private:
	class Impl;
	Impl* m_Impl;

public:

	/// \brief TODO
	Mutex();

	/// \brief TODO
	~Mutex();

	/// \brief TODO
	bool TryLock();

	/// \brief TODO
	void Lock();

	/// \brief TODO
	void Unlock();
	
};

/// \brief TODO
struct MutexLock
{
private:
	Mutex* m_Mutex;

public:

	/// \brief TODO
	MutexLock(Mutex* Lock);

	/// \brief TODO
	~MutexLock();

};

};